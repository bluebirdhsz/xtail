//
// Created by bluebird on 2016-9-15.
//

#ifndef XCONSOLE_WEB_SOCKET_H
#define XCONSOLE_WEB_SOCKET_H

#include "common.h"
#define MAX_LINE_BUF_LEN 8192
#define SHA1_LEN 20

/**
 * 返回错误消息
 */
void response_error( yile_connection_t *fd_info, const char *status, const char *msg );
void parse_protocol( yile_connection_t *fd_info );
#endif //XCONSOLE_WEB_SOCKET_H
