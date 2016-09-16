//
// Created by bluebird on 2016-9-15.
//

#include "web_socket.h"
#include "sha1.h"

/**
 * 返回错误消息
 */
void response_error( yile_connection_t *fd_info, const char *status, const char *msg ){
	char line_buf[ MAX_LINE_BUF_LEN ];
	char body_buf[ MAX_LINE_BUF_LEN ];
	const char *html_str = "<html><head><title>xtail error</title></head><body><h1>";
	sprintf( body_buf, "%s%s:%s</h1><h1>xtail</h1></body></html>", html_str, status, msg );
	yile_buf_t *buf = fd_info->read_buf;
	buf->write_pos = 0;
	sprintf( line_buf, "HTTP/1.0 %s %s\r\n", status, msg );
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	sprintf( line_buf, "Content-type: text/html\r\n" );
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	sprintf( line_buf, "Content-length: %d\r\n\r\n", strlen( body_buf ) );
	yile_buf_write( buf, (void *)line_buf, strlen( line_buf ));
	yile_buf_write( buf, body_buf, strlen( body_buf ) + 1 );
	yile_connection_send_buf_p( fd_info, buf );
	buf->read_pos = buf->write_pos;
}

/**
 * 返回swithing protocol
 */
void response_protocols( yile_connection_t *fd_info, char *secret_key ){
	char body_buf[ MAX_LINE_BUF_LEN ];
	char client_key[ MAX_LINE_BUF_LEN ];
	char server_key[ MAX_LINE_BUF_LEN ];
	const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	strcpy( client_key, secret_key );
	strcat( client_key, GUID );
	char sha_result[ SHA1_LEN ];
	printf( "%s", client_key );
	sha1_buffer( client_key, strlen(client_key), sha_result );
	base64_encode ( sha_result, SHA1_LEN, server_key );
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
 * 解析websocket协议
 */
void parse_protocol( yile_connection_t *fd_info ){
	char line_buf[ MAX_LINE_BUF_LEN ];
	const char *status = "501";
	const char *msg = "Bad Request";
	const char *flag_char = "Upgrade: websocket";
	const char *key_char = "Sec-WebSocket-Key: ";
	int i = 0, find_flag = 0;
	char *secret_key = NULL;
	yile_buf_t *buf = fd_info->read_buf;
	if ( strlen( buf->data ) != buf->write_pos ){
		return response_error( fd_info, status, msg );
	}
	if ( 0 != strncmp( "GET", (const char*)buf->data, 3 ) ){
		return response_error( fd_info, status, msg );
	}
	while( buf->read_pos < buf->write_pos && ++i < 20 ){
		if ( 0 != read_line( buf, line_buf ) ){
			return response_error( fd_info, status, msg );
		}
		if ( 0 == strncmp( flag_char, line_buf, strlen( flag_char ) ) ){
			find_flag = 1;
		}
		else if ( 0 == strncmp( key_char, line_buf, strlen( key_char ) ) ){
			secret_key = &line_buf[ strlen( key_char ) ];
			break;
		}
	}
	//没有找到key
	if ( !find_flag || NULL == secret_key ){
		return response_error( fd_info, status, msg );
	}
	printf( "Client:%d is websocket, key: %s\n", fd_info->fd, secret_key );
	response_protocols( fd_info, secret_key );
}