//
// Created by bluebird on 2016-9-14.
//

#ifndef XTAIL_XTAIL_SERVER_H
#define XTAIL_XTAIL_SERVER_H
#include "common.h"

/**
 * 打开内网监听
 */
int xtail_server_listen( const char *server_host, int server_port );

/**
 * 主进程的epoll设置
 */
int xtail_main_event_set();
#endif //XTAIL_XTAIL_SERVER_H
