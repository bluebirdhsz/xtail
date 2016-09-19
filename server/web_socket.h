//
// Created by bluebird on 2016-9-15.
//

#ifndef XCONSOLE_WEB_SOCKET_H
#define XCONSOLE_WEB_SOCKET_H

#include "common.h"
#define MAX_LINE_BUF_LEN 8192
#define SHA1_LEN 20
//读取数据返回类型
typedef enum {
	SUCCESS = 0,				//成功
	CLIENT_CLOSE,				//客户端判断
	DATA_AGAIN,					//数据未到达
	DATA_OVERFLOW				//数据太长了
} websocket_result_code;

/**
 * 返回错误消息
 */
void response_error( yile_connection_t *fd_info, const char *status, const char *msg );
/**
 * 解析websoket协议 并完成握手
 */
int websocket_handshake( yile_connection_t *fd_info );

/**
 * 根据websocket的协议解出数据
 */
websocket_result_code websocket_data_decode( yile_buf_t *read_buf, char *result, int64_t max_len, int64_t *data_len );
/**
 * 打包数据
 */
void websocket_encode_data( yile_buf_t *result, char *send_data, size_t send_len );

/**
 * 打包head（只打包head部分，然后手动拼接数据体，麻烦一些，但是性能更高）
 */
void websocket_encode_head( char *head_buf, size_t *head_len, size_t send_len );
#endif //XCONSOLE_WEB_SOCKET_H
