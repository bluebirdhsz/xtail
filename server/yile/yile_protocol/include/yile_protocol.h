#ifndef _YILE_PROTOCOL_HEAD_
#define _YILE_PROTOCOL_HEAD_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "yile_buf.h"
#include <stdint.h>
#if defined __cplusplus
#include <vector>
#include <string>
#endif

#define MAX_PROTOCOL_ID				0x7fff			//最大协议id
#define MAX_LIST_RECURSION			128				//c打印的时候需要用到

#define PROTO_UNPACK_OVERFLOW		1				//解析时溢出
#define PROTO_OUT_OF_MEMORY			2				//分配的内存不够
#define PROTO_TOO_LARGE_DATA		3				//数量太大
#define PROTO_CHECK_CODE_ERROR		4				//数据校验出错
#define PROTO_PACK_DATA_ERROR		5				//用于打包的原始数组出错
#define PROTO_UNKOWN_PACK_ID		6				//未知的协议ID

//连接类型
typedef enum {
	YILE_PROTOCOL_TYPE_C = 0,		//c语言协议
	YILE_PROTOCOL_TYPE_SO,			//php so 协议
	YILE_PROTOCOL_TYPE_CPP,			//c++协议
} yile_protocol_type;

//协议传输字符串的长度定义
typedef	uint16_t yile_protocol_str_len_t;
//协议传输字节流的长度定义
typedef	uint32_t yile_protocol_byte_len_t;
//用于for循环的变量长度
typedef uint16_t yile_protocol_list_len_t;

#if defined __cplusplus
extern "C" {
#endif
//内存池
typedef struct protocol_pool_s protocol_pool_t;
struct protocol_pool_s{
	char					*data;					//内存指针
	uint32_t				pos;					//当前位置
	uint32_t				max_size;				//最大位置
	unsigned				is_resize:1;			//是否重新分配大小
	unsigned				error_code:8;			//错误代码
};

//提供一个快速生成一个栈内存的宏
#define yile_protocol_stack_pool( pool_name, data_size )  protocol_pool_t pool_name = {0};	\
	char YILE_PROTOCOL_POOL_BUF[ data_size ];												\
	pool_name.data = YILE_PROTOCOL_POOL_BUF;												\
	pool_name.max_size = data_size

typedef void (*yile_protocol_unpack_pt)( yile_buf_t *byte_pack, protocol_pool_t *result_pool );
typedef void (*yile_protocol_pack_pt)( yile_buf_t *all_result, void *data_arr );
//协议注册
typedef struct yile_protocol_s yile_protocol_t;
struct yile_protocol_s{
	unsigned				protocol_id:16;			//协议id
	yile_protocol_type		pack_type:8;			//打包协议类型
	yile_protocol_type		unpack_type:8;			//解包协议类型
	yile_protocol_pack_pt	pack_handle;			//打包方法
	yile_protocol_unpack_pt	unpack_handle;			//解包方法
};

//带校验的包头大小
#define PROTO_CHECK_CODE_HEAD		( sizeof( packet_head_t ) + sizeof( uint32_t ) )

//尝试释放内存
#define try_free_proto_pool( p )												\
	if ( p.is_resize )															\
	{																			\
		free( p.data );															\
		p.data = NULL;															\
		p.pos = p.max_size = 0;													\
	}

//尝试释放内存
#define try_free_proto_pool_p( p )												\
	if ( p->is_resize )															\
	{																			\
		free( p->data );														\
		p->data = NULL;															\
		p->pos = p->max_size = 0;												\
	}

#pragma pack(2)
//发数据时候的数据头
typedef struct packet_head_t packet_head_t;
struct packet_head_t{
	uint32_t				size;					//数据大小
	uint16_t				pack_id;				//包类型
};
#pragma pack()

//字节流struct
typedef struct yile_protocol_byte_t yile_protocol_byte_t;
struct yile_protocol_byte_t{
	yile_protocol_byte_len_t		len;			//字节流长度
	char*							bytes;			//字节流
};

/**
 * 重置大小
 */
int yile_protocol_pool_resize( protocol_pool_t *tmp_pool, uint32_t new_size );

/**
 * 分配内存
 */
void *yile_protocol_malloc( protocol_pool_t *tmp_pool, uint32_t size );

/**
 * 不分配内存, 只做计数
 */
void yile_protocol_premalloc( protocol_pool_t *tmp_pool, uint32_t size );

/**
 * 往结果集写二进制串
 */
void yile_protocol_write_bin( yile_buf_t *pack_result, yile_protocol_byte_t *data );

/**
 * 写入字符串
 */
void yile_protocol_write_string( yile_buf_t *pack_result, const char *data );

/**
 * 读出字符串
 */
char *yile_protocol_read_string( yile_buf_t *re_pack, protocol_pool_t *data_pool );

/**
 * 从结果里读二进制串
 */
yile_protocol_byte_t *yile_protocol_read_bin( yile_buf_t *re_pack, yile_protocol_byte_t *data, protocol_pool_t *data_pool );

/**
 * 预读字符串
 */
void yile_protocol_preread_string( yile_buf_t *re_pack, protocol_pool_t *data_pool );

/**
 * 预读二进制串
 */
void yile_protocol_preread_bin( yile_buf_t *re_pack, protocol_pool_t *data_pool );

/**
 * 测试代码..打印协议数据
 */
void yile_printf_tab_string( char *re_char, int rank );

/**
 * 增加一个协议
 */
int yile_protocol_add( yile_protocol_t *tmp_protocol );

/**
 * 数据预读判断
 */
void yile_protocol_size_check( yile_buf_t *re_pack, uint32_t size );

/**
 * 读int
 */
int yile_protocol_read_int( yile_buf_t *re_pack );
/**
 * 读uint
 */
uint32_t yile_protocol_read_uint( yile_buf_t *re_pack );
/**
 * 读int16_t
 */
int16_t yile_protocol_read_short( yile_buf_t *re_pack );
/**
 * 读uint16_t
 */
uint16_t yile_protocol_read_ushort( yile_buf_t *re_pack );
/**
 * 读int8_t
 */
int8_t yile_protocol_read_byte( yile_buf_t *re_pack );
/**
 * 读uint8_t
 */
uint8_t yile_protocol_read_ubyte( yile_buf_t *re_pack );
/**
 * 读bigint
 */
int64_t yile_protocol_read_bigint( yile_buf_t *re_pack );
/**
 * 从结果里拷贝一份出来
 */
void yile_protocol_result_copy( yile_buf_t *re_pack, void *ptr, uint32_t size );

/**
 * adler32 算法
 */
uint32_t yile_protocol_check( const unsigned char *data, uint32_t len, const unsigned char *public_key, uint32_t key_len );

/**
 * 获取一个协议
 */
yile_protocol_t *yile_protocol_get( uint16_t pack_id );

#if defined __cplusplus
}
#endif
#endif