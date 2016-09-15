#ifndef YILE_NET_BASE_HEAD
#define YILE_NET_BASE_HEAD
#include "yile.h"
#include "yile_buf.h"
#include <errno.h>
//连接
typedef struct yile_connection_s yile_connection_t;
//事件回调
typedef struct yile_event_handle_s yile_event_handle_t;
//事件配置
typedef struct yile_event_conf_s yile_event_conf_t;
//事件
typedef int (*yile_action_pt)( yile_connection_t *tmp );
//读数据
typedef int (*yile_read_pt)( yile_connection_t *tmp );
//协议回调
typedef int (*yile_protocol_handle_pt)( yile_connection_t *tmp, yile_buf_t *proto_data );
#endif