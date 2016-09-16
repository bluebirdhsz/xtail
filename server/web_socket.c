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
	yile_connection_send( fd_info, body_buf, strlen( body_buf ) + 1 );
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
	char *read_data = read_buf->data;
	//可用的数据量(处理粘包，至于websocket是否要处理粘包、断包，网上暂时没找到答案)
	int available_len = read_buf->write_pos - read_buf->read_pos;
	//第一个字符
	char first_char = read_data[ 0 ];
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
	int mask = (read_data[ 1 ] & 0x7F );
	int64_t body_len;
	//如果mask 小于126，数据长度就是128
	if ( mask < 126 ){
		body_len = mask;
	}
	//如果长度 = 126 数据长度是读取接下来的 16 bit
	else if ( read_data[ 1 ] == 126 ){
		if ( available_len < 2 ){
			return DATA_AGAIN;
		}
		available_len -= 2;
		memcpy( &body_len, &read_data[ 2 ], 2 );
		read_len += 2;
	}
	//如果长度 = 127 数据长度是读取接下来的 64 bit
	else {
		if ( available_len < 8 ){
			return DATA_AGAIN;
		}
		memcpy( &body_len, &read_data[ 2 ], 4 );
		available_len -= 8;
		read_len += 8;
	}
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
	read_data += 4;
	int i;
	for ( i = 0; i < body_len; i++ ){
		read_data[ i ] ^= mask_key[ i % 4 ];
	}
	return SUCCESS;
}