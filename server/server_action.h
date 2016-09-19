//
// Created by bluebird on 2016-9-17.
//

#ifndef XCONSOLE_SERVER_ACTION_H
#define XCONSOLE_SERVER_ACTION_H
extern char *secret_key;

#include "common.h"
/**
 * 客户端（非terminal）加入服务器
 */
int action_join_xtail( yile_connection_t *fd_info, yile_buf_t *read_buf, yile_buf_t *proto_pack );

/**
 * 推送消息
 */
int action_push_msg( yile_connection_t *fd_info, yile_buf_t *read_buf );
#endif //XCONSOLE_SERVER_ACTION_H
