//
// Created by bluebird on 2016-9-14.
//

#ifndef XTAIL_XTAIL_TERMINAL_H
#define XTAIL_XTAIL_TERMINAL_H
#include "common.h"
#define IP_LEN 16
#define DECODE_DATA_BUF_LEN 1024 * 4
#define RESPONSE_DATA_BUF_LEN 8192

/**
 * terminal 初始化
 */
int xtail_terminal_init( yile_ini_t *ini_obj, const char *section );
#endif //XTAIL_XTAIL_TERMINAL_H
