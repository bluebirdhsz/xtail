#ifndef YILE_NET_CONNECTION_HEAD
#define YILE_NET_CONNECTION_HEAD
#include "yile_net_base.h"
#include "yile_protocol.h"
#include <unistd.h>
#include <sys/socket.h>

//工具的module_type 待优化
#define MODULE_TYPE_TOOL_MOD 255
#ifdef YILE_NET_DEBUG
extern uint32_t yile_net_total_send_data;
extern uint32_t yile_net_total_read_data;
#endif
typedef struct yile_connection_listen_s yile_connection_listen_t;

//释放监听fd占用
typedef int (*yile_release_listen_pt)( yile_connection_t *tmp );

//协议处理回调
typedef int (*yile_dispatch_pt)( yile_connection_t *fd_info, yile_buf_t *read_buf, uint32_t hash_id );
typedef struct yile_net_cookie_s yile_net_cookie_t;
#if defined __cplusplus
extern "C" {
#endif
#define DEFAULT_COOKIE_LEN 16
struct yile_net_cookie_s{
	uint16_t					cookie_len;		//cookie长度
	char						data_buf[ DEFAULT_COOKIE_LEN ];	//如果cookie长度小于16, 就不用再申请内存了
	char						*data;			//cookie内容
	yile_net_cookie_t			*next;			//用于对象池
};
//每一个连接结构体
struct yile_connection_s{
	uint32_t					session_id;		//会话id
	int							fd;				//文件描述符
	int							server_id;		//所在的服务器ID
	unsigned					module_type:8;	//模块类型
	unsigned					fd_type:4;		//打开的fd类型
	unsigned					is_lan:1;		//是否内网连接
	unsigned					is_close:1;		//是否关闭
	unsigned					instance:1;		//用于判断是否过期的连接
	unsigned					epoll_active:1;	//是否加入了epoll
	yile_buf_t					*read_buf;		//读数据的buf
	yile_buf_t					*write_buf;		//写数据的buf
	yile_action_pt				on_action;		//事件激活
	yile_action_pt				on_close;		//关闭
	void						*ext_data;		//该连接的附加数据
	yile_read_pt				io_read;		//读数据的方法
	yile_net_cookie_t*			cookie;			//cookie字符串
	yile_connection_t			*next;			//用于链表
};

//连接监听结构体
struct yile_connection_listen_s {
	int							listen_slot;	//监听编号
	int							port;			//端口号
	unsigned					unix_domain:1;	//是否是unix的域方式
	unsigned					round_accept:1;	//是否是轮流accept
	unsigned					round_size:4;	//每次轮流的数量
	unsigned					accept_count:4;	//当轮接收的数量
	yile_release_listen_pt		release_listen;	//释放监听fd占用
};

//连接类型
typedef enum {
	FD_TYPE_LISTEN = 0,			//主监听fd
	FD_TYPE_SOCKET,				//socket连接
	FD_TYPE_WAKEUP,				//唤醒事件fd
	FD_TYPE_TIMER,				//时间fd
	FD_TYPE_SIGNAL,				//信号fd
	FD_TYPE_CHANNEL				//socketpair fd
} yile_fd_type;

//待关闭的连接
extern yile_connection_t *yile_close_connection_list;

/**
 * 新连接
 */
yile_connection_t *yile_connection_new( int fd, yile_fd_type fd_type, yile_event_handle_t *handle );

/**
 * 关闭连接
 */
int yile_connection_close( yile_connection_t *tmp );

/**
 * 发送数据
 */
int yile_connection_send( yile_connection_t *tmp, void *data, uint32_t data_size );

/**
 * 查找一个session_id
 */
yile_connection_t *yile_connection_find( uint32_t session_id );

/**
 * 解析请求
 */
int yile_connection_request_parse( yile_connection_t *fd_info, yile_dispatch_pt action_dispatch );

/**
 * 未完成的数据
 */
packet_head_t *yile_connection_uncomplete_data();

/**
 * 设置cookie
 */
int yile_connection_set_cookie( yile_connection_t *fd_info, char *cookie, uint16_t cookie_len );

/**
 * 设置一个连接的事件
 */
void yile_connection_set_action( yile_connection_t *tmp, yile_event_handle_t *handle );

/**
 * 返回一个连接属于某个进程
 */
int yile_connection_pid( uint32_t session_id );

/**
 * 设置进程id
 */
void yile_connection_set_pid( uint8_t pid );

//便利发送数据的宏
#define yile_connection_send_buf( fd_info, buff ) yile_connection_send( fd_info, buff.data, buff.write_pos )
#define yile_connection_send_buf_p( fd_info, buff ) yile_connection_send( fd_info, buff->data, buff->write_pos )

#if defined __cplusplus
}
#endif
#endif