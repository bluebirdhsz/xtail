#ifndef YGP_PROTOCOL_SO_HEAD
#define YGP_PROTOCOL_SO_HEAD
#include <stdint.h>
#include <stdlib.h>
#include "yile_proto.h"
#include "php.h"
#pragma pack(1)

typedef struct proto_so_join_result_t proto_so_join_result_t;

typedef struct proto_so_ping_re_t proto_so_ping_re_t;

typedef struct proto_so_join_xtail_t proto_so_join_xtail_t;

typedef struct proto_so_ping_t proto_so_ping_t;

typedef struct proto_so_push_msg_t proto_so_push_msg_t;
//加入结果
struct proto_so_join_result_t{
	/** 消息码 */
	char*												message;
	/** 状态码（0表示成功） */
	uint32_t											status;
};
//ping结果
struct proto_so_ping_re_t{
	/** 消息码 */
	char*												message;
	/** 状态码（0表示成功） */
	uint32_t											status;
};
//加入服务器
struct proto_so_join_xtail_t{
	/** 验证字符串 */
	char*												auth_str;
};
//ping
struct proto_so_ping_t{
	/** 时间 */
	uint32_t											time;
};
//推送数据
struct proto_so_push_msg_t{
	/** 文件名（文件是虚拟的） */
	char*												file_name;
	/** 消息体（内容不超过65535字节） */
	char*												msg;
	/** 客户端IP地址 */
	uint32_t											ip;
	/** 消息类型：1:一般消息 2：警告消息 3：错误消息 */
	int8_t												msg_type;
};
#pragma pack()
/**
 * 打包 加入服务器
 */
void sowrite_join_xtail( yile_buf_t *pack_result, HashTable *data_hash );
/**
 * 打包 ping
 */
void sowrite_ping( yile_buf_t *pack_result, HashTable *data_hash );
/**
 * 打包 推送数据
 */
void sowrite_push_msg( yile_buf_t *pack_result, HashTable *data_hash );
/** 解析 加入结果
 */
void soread_join_result( yile_buf_t *byte_pack, zval *result_arr );
/** 解析 ping结果
 */
void soread_ping_re( yile_buf_t *byte_pack, zval *result_arr );
#endif
