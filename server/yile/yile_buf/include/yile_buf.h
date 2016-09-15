#ifndef YILE_BUFFER_HEAD
#define YILE_BUFFER_HEAD
#include "yile.h"
#include <string.h>
typedef struct yile_buf_s yile_buf_t;
typedef struct yile_buf_conf_s yile_buf_conf_t;
//提供一个快速生成一个栈内存的宏
#define yile_buf_stack_buf( buf_name, data_size )  yile_buf_t buf_name = {0};		\
	char YILE_STACK_BUF_DATA[ data_size ];											\
	buf_name.is_stack = 1;															\
	buf_name.data = YILE_STACK_BUF_DATA;											\
	buf_name.is_data_stack = 1;														\
	buf_name.max_pos = data_size

//快速生成一个栈内存空数据的buf
#define yile_buf_stack_var( buf_name ) yile_buf_t buf_name = {0};					\
	buf_name.is_stack = 1


//可写的空间大小
#define yile_buf_write_available( tmp ) ( tmp->max_pos - tmp->write_pos )

//可读的数据大小
#define yile_buf_read_available( tmp ) ( tmp->write_pos - tmp->read_pos )

//数据是否该整理了
#define yile_buf_need_trim( tmp ) ( tmp->is_resize && tmp->read_pos > 65535 && tmp->write_pos - tmp->read_pos < tmp->read_pos )

#define YILE_BUF_DATA_EOF_DATA		1	//读数据时遇到数据结束错误
#define YILE_BUF_MALLOC_ERROR		2	//内存分配出错

#if defined __cplusplus
extern "C" {
#endif
//yile_buf
struct yile_buf_s {
	uint32_t			read_pos;		//读数据开始位置
	uint32_t			write_pos;		//写数据开始位置
	uint32_t			max_pos;		//数据最大容量
	unsigned			is_stack:1;		//是否为栈内存
	unsigned			is_data_stack:1;//数据是否为栈内存
	unsigned			is_resize:1;	//是否已经分配内存
	unsigned			in_pool:1;		//是否在缓存池里
	unsigned			error_code:4;	//如果出错 出错的代码
	char				*data;			//数据
	yile_buf_t			*next;			//用于链表
};

//设置buf的
struct yile_buf_conf_s {
	uint32_t			init_data_size;	//每个缓存的初始大小
	uint32_t			max_pool_size;	//缓存池最大
	uint32_t			init_pool;		//初始化的缓存池数量
};

/**
 * 新建一个buf
 */
yile_buf_t *yile_buf_new();

/**
 * 新建一个buf 带大小
 */
yile_buf_t *yile_buf_size_new( uint32_t buf_size );

/**
 * 释放一个buf
 */
int yile_buf_free( yile_buf_t *tmp );

/**
 * 写入一些数据
 */
int yile_buf_write( yile_buf_t *tmp, void *data, uint32_t data_size );

/**
 * 扩容
 */
int yile_buf_resize( yile_buf_t *tmp, uint32_t new_size );

/**
 * 复制一个buf
 */
yile_buf_t *yile_buf_dup( yile_buf_t *tmp );

/**
 * 读一个int值
 */
int yile_buf_read_int( yile_buf_t *tmp );

/**
 * 整理一个buf
 */
void yile_buf_trim( yile_buf_t *tmp );

/**
 * 从buf里读任意数据
 */
void *yile_buf_read( yile_buf_t *tmp, uint32_t size );

/**
 * 初始化buf
 */
int yile_buf_init( yile_buf_conf_t *buf_conf );
#if defined __cplusplus
}
#endif
#endif