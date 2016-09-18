//
// Created by bluebird on 2016-9-14.
//

#include "xtail_server.h"
#include "server_action.h"
#include "proto_def.h"

yile_connection_t *join_list = NULL;
/**
 * 局域网数据分析
 */
int xtail_server_action_dispatch( yile_connection_t *fd_info , yile_buf_t *read_buf , uint32_t hash_id ){
	packet_head_t *pack_head = ( packet_head_t* )&read_buf->data[ 0 ];
	printf( "server on_action: %d %u\n\n", pack_head->pack_id, pack_head->size );
	int result = 0;
	//加入服务器
	if ( PACK_ID_JOIN_XTAIL == pack_head->pack_id ){
		yile_buf_stack_buf( proxy_data_pack, PACK_RESULT_BUF );
		result = action_join_xtail( fd_info, read_buf, &proxy_data_pack );
		if ( YILE_OK == result ){
			yile_connection_send_buf( fd_info, proxy_data_pack );
		}
		yile_buf_free( &proxy_data_pack );
	}
	else if ( PACK_ID_PUSH_MSG == pack_head->pack_id ){
		result = action_push_msg( fd_info, read_buf );
	}
	if ( YILE_OK != result ){
		yile_connection_close( fd_info );
	}
	return 0;
}

/**
 * 数据到达
 */
static int xtail_server_request( yile_connection_t *fd_info ){
	printf( "server ondata：%d %d\n\n", fd_info->read_buf->read_pos, fd_info->read_buf->write_pos );
	return yile_connection_request_parse( fd_info , xtail_server_action_dispatch );
}

/**
 * 新来连接
 */
static int xtail_server_new_conn( yile_connection_t *tmp ){
	//数据响应
	yile_event_handle_t server_data_handle = { xtail_server_request , NULL };
	yile_connection_set_action( tmp , &server_data_handle );
	//加入链表
	tmp->next = join_list;
	join_list = tmp;
	return YILE_OK;
}

/**
 * 主进程的epoll设置
 */
int xtail_main_event_set(){
	yile_event_conf_t server_event_conf = {0};
	yile_epoll_conf_t server_epoll_conf;
	server_epoll_conf.et_trigger = 1;
	server_epoll_conf.max_event = 256;
	yile_epoll_init( &server_epoll_conf, &server_event_conf );
	return YILE_OK;
}

/**
 * server 初始化
 */
int xtail_server_init( yile_ini_t *ini_obj, const char *section ){
	const char *server_host = yile_ini_get_string( section, "server_host", "127.0.0.1", ini_obj );
	int server_port = yile_ini_get_int( section, "server_port", -1, ini_obj );
	if ( -1 == server_port ){
		fprintf(stderr, "Server port read failed!\n" );
		return YILE_ERROR;
	}
	//局域网
	yile_connection_t *server_listen_fd = yile_listen_fd( server_host , server_port );
	if ( NULL == server_listen_fd ){
		fprintf( stderr , "Can not listen %s:%d\n" , server_host , server_port );
		return YILE_ERROR;
	}
	//局域网通信处理
	yile_event_handle_t server_listen_handle = { xtail_server_new_conn, NULL };
	//设置事件
	yile_connection_set_action( server_listen_fd , &server_listen_handle );

	//加入密码
	const char *join_key = yile_ini_get_string( section, "secret", "www.ffan.com", ini_obj );
	secret_key = strdup( join_key );
	return YILE_OK;
}