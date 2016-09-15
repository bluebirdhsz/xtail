//
// Created by bluebird on 2016-9-14.
//

#include "xtail_server.h"

/**
 * 局域网数据分析
 */
int xtail_server_action_dispatch( yile_connection_t *fd_info , yile_buf_t *read_buf , uint32_t hash_id ){
	return 0;
}

/**
 * 数据到达
 */
static int xtail_server_request( yile_connection_t *fd_info ){
	printf( "xtail_server_request\n" );
	return yile_connection_request_parse( fd_info , xtail_server_action_dispatch );
}

/**
 * 新来连接
 */
static int xtail_server_new_conn( yile_connection_t *tmp ){
	//数据响应
	yile_event_handle_t server_data_handle = { xtail_server_request , NULL };
	yile_connection_set_action( tmp , &server_data_handle );
	return YILE_OK;
}

/**
 * 打开内网监听
 */
int xtail_server_listen( const char *server_host, int server_port ){
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
}