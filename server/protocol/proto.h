#ifndef YILE_PROTOCOL_PROTO_H
#define YILE_PROTOCOL_PROTO_H
#include <stdint.h>
#include <stdlib.h>
#include "yile_protocol.h"
#if defined __cplusplus
extern "C" {
#endif

typedef struct proto_join_result_t proto_join_result_t;

typedef struct proto_ping_re_t proto_ping_re_t;

typedef struct proto_join_xtail_t proto_join_xtail_t;

typedef struct proto_ping_t proto_ping_t;

typedef struct proto_push_msg_t proto_push_msg_t;
//加入结果
struct proto_join_result_t{
	/** 消息码 */
	char*												message;
	/** 状态码（0表示成功） */
	uint32_t											status;
};
//ping结果
struct proto_ping_re_t{
	/** 消息码 */
	char*												message;
	/** 状态码（0表示成功） */
	uint32_t											status;
};
//加入服务器
struct proto_join_xtail_t{
	/** 验证字符串 */
	char*												auth_str;
};
//ping
struct proto_ping_t{
	/** 时间 */
	uint32_t											time;
};
//推送数据
struct proto_push_msg_t{
	/** 文件名（文件是虚拟的） */
	char*												file_name;
	/** 消息体（内容不超过65535字节） */
	char*												msg;
	/** 客户端IP地址 */
	uint32_t											ip;
	/** 消息类型：1:一般消息 2：警告消息 3：错误消息 */
	int8_t												msg_type;
};

/**
 * 生成 加入结果
 */
void write_join_result( yile_buf_t *pack_result, proto_join_result_t *data_arr );

/**
 * 生成 ping结果
 */
void write_ping_re( yile_buf_t *pack_result, proto_ping_re_t *data_arr );

/**
 * 解析 加入服务器
 */
proto_join_xtail_t *read_join_xtail( yile_buf_t *byte_pack, protocol_pool_t *result_pool );

/**
 * 解析 ping
 */
proto_ping_t *read_ping( yile_buf_t *byte_pack, protocol_pool_t *result_pool );

/**
 * 解析 推送数据
 */
proto_push_msg_t *read_push_msg( yile_buf_t *byte_pack, protocol_pool_t *result_pool );
/**
 * 计算解析内存 加入服务器
 */
uint32_t size_read_join_xtail( yile_buf_t *byte_pack );
/**
 * 计算解析内存 推送数据
 */
uint32_t size_read_push_msg( yile_buf_t *byte_pack );
#ifdef PROTOCOL_DEBUG

/**
 * 打印 加入服务器
 */
void print_join_xtail( proto_join_xtail_t *re );

/**
 * 打印 ping
 */
void print_ping( proto_ping_t *re );

/**
 * 打印 推送数据
 */
void print_push_msg( proto_push_msg_t *re );
#endif
#if defined __cplusplus
}
#endif
#endif
