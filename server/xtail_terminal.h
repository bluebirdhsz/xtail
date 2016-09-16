//
// Created by bluebird on 2016-9-14.
//

#ifndef XTAIL_XTAIL_TERMINAL_H
#define XTAIL_XTAIL_TERMINAL_H
#include "common.h"
#define IP_LEN 16
#define MAX_ACTION_LEN 256
#define DECODE_DATA_BUF_LEN 40960

/**
 * terminal 初始化
 */
int xtail_terminal_init( yile_ini_t *ini_obj, const char *section );
#endif //XTAIL_XTAIL_TERMINAL_H
