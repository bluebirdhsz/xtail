//
// Created by bluebird on 2016-9-17.
//

#include <yile_net/include/yile_net.h>
#include "server_action.h"
#include "proto.h"
#include "proto_def.h"
#include "terminal_group.h"
char *secret_key;
/**
 * 客户端（非terminal）加入服务器
 */
int action_join_xtail( yile_connection_t *fd_info, yile_buf_t *read_buf, yile_buf_t *proto_pack ){
	read_buf->read_pos = sizeof( packet_head_t );
	yile_protocol_stack_pool( proto_result, PROTO_SIZE_JOIN_XTAIL );
	proto_join_xtail_t *join_xtail = read_join_xtail( read_buf, &proto_result );
	if ( NULL == join_xtail ){
		try_free_proto_pool( proto_result );
		return YILE_ERROR;
	}
	proto_join_result_t join_result;
	if ( 0 != strcmp( secret_key, join_xtail->auth_str ) ){
		join_result.status = 1;
		join_result.message = "auth key error!";
	}
	else{
		join_result.status = 0;
		join_result.message = "success";
		fd_info->is_lan = 1;
	}
	write_join_result( proto_pack, &join_result );
	try_free_proto_pool( proto_result );
	return YILE_OK;
}

/**
 * 推送消息
 */
int action_push_msg( yile_connection_t *fd_info, yile_buf_t *read_buf ){
	read_buf->read_pos = sizeof( packet_head_t );
	yile_protocol_stack_pool( proto_result, PROTO_SIZE_PUSH_MSG );
	uint32_t need_size = size_read_push_msg( read_buf );
	if ( need_size < PROTO_SIZE_PUSH_MSG ){
		yile_protocol_pool_resize( &proto_result, need_size );
	}
	proto_push_msg_t *msg_pack = read_push_msg( read_buf, &proto_result );
	if ( NULL == msg_pack ){
		try_free_proto_pool( proto_result );
		return YILE_ERROR;
	}
	terminal_group_push_msg( msg_pack );
	return YILE_OK;
}