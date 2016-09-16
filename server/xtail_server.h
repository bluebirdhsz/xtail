//
// Created by bluebird on 2016-9-14.
//

#ifndef XTAIL_XTAIL_SERVER_H
#define XTAIL_XTAIL_SERVER_H
#include "common.h"

/**
 * server 初始化
 */
int xtail_server_init( yile_ini_t *ini_obj, const char *section );

/**
 * 主进程的epoll设置
 */
int xtail_main_event_set();
#endif //XTAIL_XTAIL_SERVER_H
