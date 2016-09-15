#ifndef YILE_NET_EPOLL_HEAD
#define YILE_NET_EPOLL_HEAD
#include "yile_net_base.h"
#include <sys/epoll.h>

typedef struct yile_epoll_conf_s yile_epoll_conf_t;

//是否使用ET触发模式
extern int yile_epoll_use_et;

#if defined __cplusplus
extern "C" {
#endif
//epoll配置
struct yile_epoll_conf_s {
	int			et_trigger;		//是否使用et模式
	int			max_event;		//最大事件个数
};

/**
 * epoll初始化
 */
int yile_epoll_init( yile_epoll_conf_t *epoll_conf, yile_event_conf_t *event_conf );

/**
 * 增加一个连接
 */
int yile_epoll_add_connection( yile_connection_t *tmp );

/**
 * 更新一个连接事件
 */
int yile_epoll_update_connection( yile_connection_t *tmp, int new_event );

/**
 * 从epoll移除一个连接
 */
int yile_epoll_del_connection( yile_connection_t *tmp );

/**
 * 等待事件发生
 */
int yile_epoll_loop();

/**
 * epoll是否已经初始化
 */
int yile_epoll_is_init();

/**
 * 关闭epoll
 */
int yile_epoll_close();
#if defined __cplusplus
}
#endif
#endif