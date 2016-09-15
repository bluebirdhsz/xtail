//
// Created by bluebird on 2016-9-14.
//

#ifndef XTAIL_COMMON_H
#define XTAIL_COMMON_H
#include "yile.h"
#include "yile_buf.h"
#include "yile_net.h"
#include "yile_ini.h"
#include "yile_log.h"
#include "yile_process.h"
#include "yile_protocol.h"

//最大数据包
#define YILE_ROUTE_PACK_MAX_SIZE 0xA00000
//子进程个数
extern int xtail_process_num;
#endif //XTAIL_COMMON_H
