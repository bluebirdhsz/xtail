//
// Created by bluebird on 2016-9-15.
//

#include "web_socket.h"
#include "sha1.h"
#include "utils.h"

/**
 * 返回错误消息
 */
void response_error( yile_connection_t *fd_info, const char *status, const char *msg ){
	char line_buf[MAX_LINE_BUF_LEN];
	char body_buf[MAX_LINE_BUF_LEN];
	const char *html_str = "<html><head><title>xtail error</title></head><body><h1>";
	sprintf( body_buf, "%s%s:%s</h1><h1>xtail</h1></body></html>", html_str, status, msg );
	yile_buf_t *buf = fd_info->read_buf;
	buf->write_pos = 0;
	sprintf( line_buf, "HTTP/1.0 %s %s\r\n", status, msg );
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	sprintf( line_buf, "Content-type: text/html\r\n" );
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	sprintf( line_buf, "Content-length: %d\r\n\r\n", strlen( body_buf ));
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	yile_buf_write( buf, body_buf, strlen( body_buf ) + 1 );
	yile_connection_send_buf_p( fd_info, buf );
	buf->read_pos = buf->write_pos;
}

/**
 * 返回swithing protocol
 */
void response_protocols( yile_connection_t *fd_info, char *secret_key ){
	char body_buf[MAX_LINE_BUF_LEN];
	char client_key[MAX_LINE_BUF_LEN];
	char server_key[MAX_LINE_BUF_LEN];
	const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	strcpy( client_key, secret_key );
	strcat( client_key, GUID );
	char sha_result[SHA1_LEN];
	printf( "%s", client_key );
	sha1_buffer( client_key, strlen( client_key ), sha_result );
	base64_encode( sha_result, SHA1_LEN, server_key );
	sprintf( body_buf, "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n" );
	sprintf( &body_buf[ strlen( body_buf ) ], "Sec-WebSocket-Accept: %s\r\n\r\n", server_key );
	printf( "send:%s", body_buf );
	//这里发送数据的时候，不能把最后一个 \0 发送回客户端，它会影响下一次接收数据包，已经吃坑！
	yile_connection_send( fd_info, body_buf, strlen( body_buf ) );
}

/**
 * 读出一行
 */
int read_line( yile_buf_t *buf, char *result ){
	int result_i = 0;
	uint32_t i;
	for ( i = buf->read_pos; i < buf->write_pos; ){
		char c = buf->data[ i++ ];
		if ( '\r' == c ){
			//++i 是\n
			++i;
			result[ result_i ] = '\0';
			break;
		}
		else{
			result[ result_i++ ] = c;
			if ( result_i >= MAX_LINE_BUF_LEN ){
				return 1;
			}
		}
	}
	buf->read_pos = i;
	return 0;
}

/**
 * 解析websoket协议 并完成握手
 */
int websocket_handshake( yile_connection_t *fd_info ){
	char line_buf[MAX_LINE_BUF_LEN];
	const char *status = "501";
	const char *msg = "Bad Request";
	const char *flag_char = "Upgrade: websocket";
	const char *key_char = "Sec-WebSocket-Key: ";
	int i = 0, find_flag = 0;
	char *secret_key = NULL;
	yile_buf_t *buf = fd_info->read_buf;
	//写入一个结束标志
	yile_buf_write( buf, "", 1 );
	if ( strlen( buf->data ) != buf->write_pos - 1 ){
		response_error( fd_info, status, msg );
		return YILE_ERROR;
	}
	if ( 0 != strncmp( "GET", (const char *)buf->data, 3 )){
		response_error( fd_info, status, msg );
		return YILE_ERROR;
	}
	while ( buf->read_pos < buf->write_pos && ++i < 20 ){
		if ( 0 != read_line( buf, line_buf )){
			response_error( fd_info, status, msg );
			return YILE_ERROR;
		}
		if ( 0 == strncmp( flag_char, line_buf, strlen( flag_char ))){
			find_flag = 1;
		}
		else if ( 0 == strncmp( key_char, line_buf, strlen( key_char ))){
			secret_key = &line_buf[ strlen( key_char ) ];
			break;
		}
	}
	//没有找到key
	if ( !find_flag || NULL == secret_key ){
		response_error( fd_info, status, msg );
		return YILE_ERROR;
	}
	printf( "Client:%d is websocket, key: %s\n", fd_info->fd, secret_key );
	response_protocols( fd_info, secret_key );
	//标志数据已经读完，不然会影响下一次读取
	buf->read_pos = buf->write_pos;
	return YILE_OK;
}

/**
 * 根据websocket的协议解出数据
 *  0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
 */
websocket_result_code websocket_data_decode( yile_buf_t *read_buf, char *result, int64_t max_len, int64_t *data_len ){
	char *read_data = &read_buf->data[ read_buf->read_pos ];
	//printf( "数据到达: read_pos:%d write_pos: %d \n\n", read_buf->read_pos, read_buf->write_pos );
	//可用的数据量(处理粘包，至于websocket是否要处理粘包、断包，网上暂时没找到答案)
	int available_len = read_buf->write_pos - read_buf->read_pos;
	//第一个字符
	char first_char = read_data[ 0 ];
	/*
	 * @link https://tools.ietf.org/html/rfc6455#section-5.4
	 * todo 根据规范的5.4节描述，应该先处理第1个bit FIN，如果为1表示是完整数据包， 如果是0：表示后面还有数据
	 * 回头再完善。
	 */
	//客户端关闭
	if ( first_char & 0x8 ){
		return CLIENT_CLOSE;
	}
	//数据长度小于最小协议头（下次处理）
	if ( available_len < 6 ){
		return DATA_AGAIN;
	}
	//标记可用空间减少最小协议头长度
	available_len -= 6;
	int read_len = 2;
	//第二个字节
	int len_flag = (read_data[ 1 ] & 0x7F );
	/**
	 * todo 根据协议，要先判断mask的第1个bit是否为1，如果不为1，应该踢掉链接
	 */
	int64_t body_len = 0;
	//如果mask 小于126，数据长度就是128
	if ( len_flag < 126 ){
		body_len = len_flag;
	}
	//如果长度 = 126 数据长度是读取接下来的 16 bit
	else if ( len_flag == 126 ){
		if ( available_len < 2 ){
			return DATA_AGAIN;
		}
		//这里要将收到的网络字节序 转换成 机器字节序
		uint16_t tmp_body_len = 0;
		memcpy( &tmp_body_len, &read_data[ 2 ], sizeof( tmp_body_len ) );
		body_len = ntohs( tmp_body_len );
		available_len -= 2;
		read_len += 2;
	}
	//如果长度 = 127 数据长度是读取接下来的 64 bit
	else {
		if ( available_len < 8 ){
			return DATA_AGAIN;
		}
		uint64_t tmp_body_len = 0;
		memcpy( &tmp_body_len, &read_data[ 2 ], 8 );
		body_len = ntoh64( tmp_body_len );
		available_len -= 8;
		read_len += 8;
	}
	//printf( "body_len: %u max_len: %u available: %u \n\n", body_len, max_len, available_len );
	if ( body_len >= max_len ){
		return DATA_OVERFLOW;
	}
	//数据包体长度不够，可能是断包，等待下一次数据到达
	if ( available_len < body_len ){
		return DATA_AGAIN;
	}
	char *mask_key = &read_data[ read_len ];
	*data_len = body_len;
	//mask_key的长度
	read_len += 4;
	read_buf->read_pos += read_len + body_len;
	//跳到数据体开始位置
	read_data += read_len;
	int i;
	for ( i = 0; i < body_len; i++ ){
		read_data[ i ] ^= mask_key[ i % 4 ];
	}
	memcpy( result, read_data, (size_t)body_len );
	result[ body_len ] = '\0';
	//printf( "receive data: %s\n\n", result );
	return SUCCESS;
}

/**
 * 打包数据
 */
void websocket_encode_data( yile_buf_t *result, char *send_data, size_t send_len ){
	//printf( "send data:%s len(%d) \n\n", send_data, send_len );
	//0x81表示是一个完整的文本数据包（以后再重构成支持多种数据类型的）
	yile_buf_write_byte( result, (char)0x81 );
	//返回的数据可以不加mask
	char mask = 0;
	//内容长度在0-125
	if ( send_len < 126 ){
		mask = (char)send_len;
		yile_buf_write_byte( result, mask );
	}
	//内容长度小于65535
	else if ( send_len <= 0xFFFF ){
		yile_buf_write_byte( result, 126 );
		uint16_t tmp_data_len = (uint16_t)send_len;
		//这里要将 机器字节序转换成 网络字节序
		tmp_data_len = htons( tmp_data_len );
		yile_buf_write( result, &tmp_data_len, sizeof( tmp_data_len ) );
	}
	//内容长度大于65535
	else{
		yile_buf_write_byte( result, 127 );
		uint64_t tmp_data_len = send_len;
		tmp_data_len = hton64( tmp_data_len );
		yile_buf_write( result, &tmp_data_len, sizeof( tmp_data_len ) );
	}
	yile_buf_write( result, send_data, send_len );
}

/**
 * 打包head（只打包head部分，然后手动拼接数据体，麻烦一些，但是性能更高）
 */
void websocket_encode_head( char *head_buf, size_t *head_len, size_t send_len ){
	size_t total_len = 2;
	head_buf[ 0 ] = (char)0x81;
	if ( send_len < 126 ){
		head_buf[ 1 ] = (char)send_len;
	}
	//内容长度小于65535
	else if ( send_len <= 0xFFFF ){
		head_buf[ 1 ] = 126;
		uint16_t tmp_data_len = (uint16_t)send_len;
		//这里要将 机器字节序转换成 网络字节序
		tmp_data_len = htons( tmp_data_len );
		head_buf += total_len;
		memcpy( head_buf, &tmp_data_len, sizeof( tmp_data_len ) );
		total_len += sizeof( tmp_data_len );
	}
	//内容长度大于65535
	else{
		head_buf[ 1 ] = 127;
		uint64_t tmp_data_len = (uint64_t)send_len;
		//这里要将 机器字节序转换成 网络字节序
		tmp_data_len = hton64( tmp_data_len );
		head_buf += total_len;
		memcpy( head_buf, &tmp_data_len, sizeof( tmp_data_len ) );
		total_len += sizeof( tmp_data_len );
	}
	*head_len = total_len;
}