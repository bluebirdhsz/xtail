#ifndef YILE_NET_EVENT_HEAD
#define YILE_NET_EVENT_HEAD
#include "yile_net_base.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>

#define YILE_READ_EVENT EPOLLIN|EPOLLRDHUP
#define YILE_WRITE_EVENT EPOLLOUT|EPOLLRDHUP
//判断一个定时器是不是interval的
#define YILE_TIMER_IS_INTERVAL( tmp ) ( 1 == tmp->module_type )

#if defined __cplusplus
extern "C" {
#endif
//事件处理句柄
struct yile_event_handle_s{
	yile_action_pt		on_action;		//数据包到达
	yile_action_pt		on_close;		//关闭
};
//事件配置
struct yile_event_conf_s{
	yile_action_pt		on_accept;			//有连接到来的回调
	yile_action_pt		on_data;			//socket数据到达
	yile_action_pt		on_timeup;			//倒计时结束
	yile_action_pt		on_signal;			//信号到达
	yile_action_pt		on_wakeup;			//唤醒回调
	yile_action_pt		listenfd_close;		//监听fd关闭
	yile_action_pt		socketfd_close;		//socke tfd关闭
	yile_action_pt		timerfd_close;		//定时fd关闭
	yile_action_pt		signalfd_close;		//信号fd关闭
	yile_action_pt		wakeupfd_close;		//唤醒fd关闭
};

/**
 * 初始化事件
 */
int yile_event_init( yile_event_conf_t *event_conf );

/**
 * 创建一个监听端口
 */
yile_connection_t *yile_listen_fd( const char *host, int port );

/**
 * 创建一个唤醒事件
 */
yile_connection_t *yile_wakeup_fd();

/**
 * 创建一个定时器
 */
yile_connection_t *yile_timer_fd();

/**
 * 创建一个信号捕捉
 */
yile_connection_t *yile_signal_fd( int *signals, int signal_num );

/**
 * 新来的socket
 */
yile_connection_t *yile_socket_fd( int fd );

/**
 * 创建一个socketfd客户端
 */
yile_connection_t *yile_socket_client_fd( const char *host, int port );

/**
 * 设置倒计时(单次)
 */
int yile_set_timeout( yile_connection_t *tmp, int micsecond, yile_action_pt callback );

/**
 * 设置倒计时(一直)
 */
int yile_set_interval( yile_connection_t *tmp, int micsecond, yile_action_pt callback );

/**
 * 写socket数据
 */
int yile_event_socket_write( yile_connection_t *tmp );

/**
 * 唤醒一个事件
 */
int yile_event_wakeup( yile_connection_t *tmp );

/**
 * 读socket数据
 */
int yile_event_socket_read( yile_connection_t *tmp );

/**
 * 重新初始化一个连接的事件
 */
void yile_event_recover( yile_connection_t *tmp );
#if defined __cplusplus
}
#endif

#endif