//
// Created by bluebird on 2016-9-14.
//

#include "xtail_terminal.h"
#include "web_socket.h"
/**
 * 局域网数据分析
 */
int xtail_terminal_action_dispatch( yile_connection_t *fd_info , yile_buf_t *read_buf , uint32_t hash_id ){
	return 0;
}

/**
 * 数据到达
 */
static int xtail_terminal_request( yile_connection_t *fd_info ){
	//没有ext_data 表示还没有正式加入
	if ( NULL == fd_info->ext_data ){
		parse_protocol( fd_info );
	}
	return yile_connection_request_parse( fd_info , xtail_terminal_action_dispatch );
}

/**
 * 连接关闭
 */
static int xtail_terminal_close( yile_connection_t *tmp ){
	printf( "fd %d close\n", tmp->fd );
	return YILE_OK;
}

/**
 * 新来连接
 */
static int xtail_terminal_new_conn( yile_connection_t *tmp ){
	//数据响应
	yile_event_handle_t server_data_handle = { xtail_terminal_request , xtail_terminal_close };
	yile_connection_set_action( tmp , &server_data_handle );
	return YILE_OK;
}

/**
 * 主通信端口断开
 */
static int xtail_terminal_listen_close( yile_connection_t *tmp ){
	fprintf( stderr , "xtail terminal listen close\n" );
	return YILE_OK;
}


/**
 * 打开终端监听
 */
int xtail_terminal_listen( const char *terminal_host, int terminal_port ){
	//局域网
	yile_connection_t *terminal_listen_fd = yile_listen_fd( terminal_host , terminal_port );
	if ( NULL == terminal_listen_fd ){
		fprintf( stderr , "Can not listen %s:%d\n" , terminal_host , terminal_port );
		return YILE_ERROR;
	}
	//局域网通信处理
	yile_event_handle_t terminal_listen_handle = { xtail_terminal_new_conn , xtail_terminal_listen_close };
	//设置事件
	yile_connection_set_action( terminal_listen_fd , &terminal_listen_handle );
	return YILE_OK;
}