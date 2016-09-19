//
// Created by bluebird on 2016-9-17.
//

#include "server_action.h"
#include "proto.h"
#include "proto_def.h"
#include "terminal_group.h"
#include "web_socket.h"

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
	//以下代码为了性能考虑，直接修改 read_buf->data 然后于用广播，可读性很差，但是性能提升蛮多
	uint32_t beg_pos =  sizeof( packet_head_t ) + sizeof( msg_pack->group_id );
	//msg_type只占一位
	beg_pos += sizeof( char );
	//hostname在buf里占的空间
	size_t hostname_len = strlen( msg_pack->host_name );
	beg_pos += sizeof( uint16_t ) + hostname_len;
	//消息内容的长度在buf里占的空间
	beg_pos += sizeof( uint32_t );

	char *buf_data = read_buf->data;
	//首先，在消息内容前1位加上 '|'
	buf_data[ --beg_pos ] = '|';
	//再把hostname写进来
	memcpy( &buf_data[ beg_pos - hostname_len ], msg_pack->host_name, hostname_len );
	beg_pos -= hostname_len;
	//第三步再次写入'|'
	buf_data[ --beg_pos ] = '|';
	//最后一步，写入msg_type
	char tmp_buf[ 10 ];
	int tmp_type = msg_pack->msg_type;
	sprintf( tmp_buf, "%d", tmp_type );
	size_t type_char_len = strlen( tmp_buf );
	memcpy( &buf_data[ beg_pos - type_char_len ], tmp_buf, type_char_len );
	beg_pos -= type_char_len;
	size_t wbs_head_len = 0;
	//生成一个websocket header
	websocket_encode_head( tmp_buf, &wbs_head_len, read_buf->write_pos - beg_pos );
	//再将这个head写入待发送区
	memcpy( &buf_data[ beg_pos - wbs_head_len ], tmp_buf, wbs_head_len );
	beg_pos -= wbs_head_len;
	//最后再组内广播消息
	terminal_group_send_msg( msg_pack->group_id, &buf_data[ beg_pos ], read_buf->write_pos - beg_pos );
	try_free_proto_pool( proto_result );
	return YILE_OK;
}