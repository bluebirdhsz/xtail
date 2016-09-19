//
// Created by bluebird on 2016-9-16.
//

#ifndef XCONSOLE_TERMINAL_GROUP_H
#define XCONSOLE_TERMINAL_GROUP_H

#include "common.h"
//默认的分组hash大小
#define DEFAULT_GROUP_HASH_SIZE 33
#define MAX_GROUP_HASH_SIZE 2500
//终端分组（支持多个终端同时tail相同消息）
typedef struct terminal_group_s terminal_group_t;
//终端
typedef struct terminal_client_s terminal_client_t;

//小组
struct terminal_group_s{
	int								client_size;
	uint32_t						group_id;
	terminal_client_t				*client_list;
	terminal_group_t				*next;
};
//成员
struct terminal_client_s{
	time_t 							ping_time;
	terminal_group_t				*group_info;
	yile_connection_t				*fd_info;
	terminal_client_t				*last;
	terminal_client_t				*next;
};

/**
 * 将一个终端加入IP组
 */
void terminal_group_add_client( yile_connection_t *fd_info, uint32_t group_id );

/**
 * 将一个终端移出一个组
 */
void terminal_group_remove_client( yile_connection_t *fd_info );

/**
 * 将消息广播给组
 */
void terminal_group_send_msg( uint32_t group_id, char *send_data, size_t send_len );

/**
 * 踢出不活动的terminal client
 */
int terminal_group_kick_idle( yile_connection_t *timer_fd );
#endif //XCONSOLE_TERMINAL_GROUP_H
