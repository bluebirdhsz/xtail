//
// Created by bluebird on 2016-9-14.
//

#include "xtail_terminal.h"
#include "web_socket.h"
#include "utils.h"
#include "terminal_group.h"

/**
 * action路由
 * todo 由于目前terminal只有ping请求，如果请求多的话，应该写一个action hash map 或者二叉树
 */
int xtail_terminal_action_dispatch( yile_connection_t *fd_info, char *data ){
	char *action_name = data;
	char *data_body = strchr( data, '|' );
	//没有|字符串，不识别
	if ( NULL == data_body ){
		return YILE_ERROR;
	}
	//将|替换成'\0'，这样就把data变成了 action_name和data_body两个安全的字符串
	data_body[ 0 ] = '\0';
	data_body++;
	//不识别的action 关闭当前连接
	if ( 0 != strcmp( action_name, "ping" ) ){
		return YILE_ERROR;
	}
	printf( "data:%s\n", data_body );
	return YILE_OK;
}

/**
 * 新的websocket客户端
 */
static void new_websocket_accept( yile_connection_t *fd_info ){
	int re = websocket_handshake( fd_info );
	//握手失败
	if ( YILE_ERROR == re ){
		yile_connection_close( fd_info );
		return;
	}
	//检测客户端指定的IP，如果没有指定，就使用当前连接的ip
	char ip_buf[IP_LEN];
	char *assign_ip = str_search( fd_info->read_buf->data, "/?ip=", " ", ip_buf, IP_LEN );
	uint32_t ip_add = 0;
	//如果客户端指了IP
	if ( NULL != assign_ip ){
		printf( "client %d 指定IP:%s\n\n", fd_info->fd, assign_ip );
		struct in_addr client_ip;
		if ( 1 == inet_pton( AF_INET, assign_ip, &client_ip )){
			ip_add = ntohl( client_ip.s_addr );
		}
	}
	//没有指定 ip 或者 指定的ip地址不正确
	if ( 0 == ip_add ){
		struct sockaddr_in client_addr;
		client_addr.sin_addr.s_addr = 0;
		char guest_ip[20];
		int guest_len = sizeof( client_addr );
		getpeername( fd_info->fd, (struct sockaddr *)&client_addr, (socklen_t * ) & guest_len );
		ip_add = htonl( client_addr.sin_addr.s_addr );
	}
	printf( "Ip %u \n", ip_add );
	//将新来的terminal加入分组，组以ip的标识，这么做是为了支持多个terminal tail同一个ip
	terminal_group_add_client( fd_info, ip_add );
}

/**
 * 数据到达
 */
static int xtail_terminal_request( yile_connection_t *fd_info ){
	//没有ext_data 表示还没有正式加入
	if ( NULL == fd_info->ext_data ){
		new_websocket_accept( fd_info );

	}
	else{
		yile_buf_t *buf = fd_info->read_buf;
		//有可能一次会发很多个数据包过来（粘包处理）
		while( yile_buf_read_available( buf ) > 0 ){
			int64_t data_len = 0;
			char result_buf[ DECODE_DATA_BUF_LEN ];
			websocket_result_code ret =  websocket_data_decode( buf, result_buf, DECODE_DATA_BUF_LEN, &data_len );
			if ( CLIENT_CLOSE == ret || DATA_OVERFLOW == ret ){
				yile_connection_close( fd_info );
				break;
			}
			//等待下次数据到达
			if ( DATA_AGAIN == ret ){
				break;
			}
			result_buf[ data_len ] = '\0';
			if ( YILE_ERROR == xtail_terminal_action_dispatch( fd_info, result_buf ) ){
				yile_connection_close( fd_info );
				break;
			}
		}
	}
	return YILE_OK;
}

/**
 * 连接关闭
 */
static int xtail_terminal_close( yile_connection_t *fd_info ){
	printf( "fd %d close\n", fd_info->fd );
	terminal_group_remove_client( fd_info );
	fd_info->ext_data = NULL;
	return YILE_OK;
}

/**
 * 新来连接
 */
static int xtail_terminal_new_conn( yile_connection_t *fd_info ){
	//数据响应
	yile_event_handle_t server_data_handle = {xtail_terminal_request, xtail_terminal_close};
	yile_connection_set_action( fd_info, &server_data_handle );
	return YILE_OK;
}

/**
 * 主通信端口断开
 */
static int xtail_terminal_listen_close( yile_connection_t *tmp ){
	fprintf(stderr, "xtail terminal listen close\n" );
	return YILE_OK;
}

/**
 * terminal 初始化
 */
int xtail_terminal_init( yile_ini_t *ini_obj, const char *section ){
	const char *terminal_host = yile_ini_get_string( section, "terminal_host", "0.0.0.0", ini_obj );
	int terminal_port = yile_ini_get_int( section, "terminal_port", -1, ini_obj );
	if ( -1 == terminal_port ){
		fprintf(stderr, "Terminal port read failed!\n" );
		return YILE_ERROR;
	}
	//局域网
	yile_connection_t *terminal_listen_fd = yile_listen_fd( terminal_host, terminal_port );
	if ( NULL == terminal_listen_fd ){
		fprintf(stderr, "Can not listen %s:%d\n", terminal_host, terminal_port );
		return YILE_ERROR;
	}
	//局域网通信处理
	yile_event_handle_t terminal_listen_handle = {xtail_terminal_new_conn, xtail_terminal_listen_close};
	//设置事件
	yile_connection_set_action( terminal_listen_fd, &terminal_listen_handle );
	//设置一个定时器，将没有ping的客户端踢出
	yile_connection_t *timer_fd = yile_timer_fd();
	int ping_interval = yile_ini_get_int( section, "ping_interval", 30, ini_obj );
	//每一定时间
	yile_set_interval( timer_fd, ping_interval, terminal_group_kick_idle );
	return YILE_OK;
}