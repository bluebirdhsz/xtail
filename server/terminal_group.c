//
// Created by bluebird on 2016-9-16.
//

#include "terminal_group.h"

//组hash map 以ip地址的网络字节顺序为hash key 因为是uint32的，直接mod性能非常高
static terminal_group_t **group_hash_map;

//组列表数量
static int group_list_hash_size;

//当前list的数量
static int group_list_size;

//空闲组
static terminal_group_t *free_group_head;

//空闲组数量
static int free_group_num;

//空闲组最大数量
const static int free_group_max_num = 100;

//空闲成员
static terminal_client_t *free_group_member_head;

//空闲成员数量
static int free_group_member_num;

//空闲最大成员数量
const static int free_group_member_max_num = 200;

/**
 * 新建一个组
 */
static terminal_group_t *yile_route_group_new(){
	terminal_group_t *tmp;
	if ( free_group_num > 0 ){
		tmp = free_group_head;
		free_group_head = free_group_head->next;
		--free_group_num;
	}
	else{
		tmp = (terminal_group_t *)malloc( sizeof( terminal_group_t ));
		if ( NULL == tmp ){
			fprintf(stderr, "yile_route_group_new() out of memory\n" );
			return NULL;
		}
	}
	memset( tmp, 0, sizeof( terminal_group_t ));
	return tmp;
}

/**
 * 释放一个组
 */
static void yile_route_group_free( terminal_group_t *tmp ){
	if ( free_group_num >= free_group_max_num ){
		free( tmp );
	}
	else{
		tmp->next = free_group_head;
		free_group_head = tmp;
		++free_group_num;
	}
}

/**
 * 新建一个client成员
 */
static terminal_client_t *terminal_group_client_new(){
	terminal_client_t *tmp;
	if ( free_group_member_num > 0 ){
		tmp = free_group_member_head;
		free_group_member_head = free_group_member_head->next;
		--free_group_member_num;
	}
	else{
		tmp = (terminal_client_t *)malloc( sizeof( terminal_client_t ));
		if ( NULL == tmp ){
			fprintf(stderr, "terminal_group_client_new() out of memory\n" );
			return NULL;
		}
	}
	memset( tmp, 0, sizeof( terminal_client_t ));
	return tmp;
}

/**
 * 释放一个client
 */
static void terminal_group_client_free( terminal_client_t *tmp ){
	if ( free_group_member_num >= free_group_member_max_num ){
		free( tmp );
	}
	else{
		tmp->next = free_group_member_head;
		free_group_member_head = tmp;
		++free_group_member_num;
	}
}

/**
 * 查找一个group
 */
static terminal_group_t *terminal_group_find( uint32_t group_id ){
	if ( 0 == group_list_hash_size ){
		return NULL;
	}
	int index = group_id % group_list_hash_size;
	if ( NULL == group_hash_map[ index ] ){
		return NULL;
	}
	terminal_group_t *tmp = group_hash_map[ index ];
	if ( group_hash_map[ index ]->group_id == group_id ){
		return tmp;
	}
	else{
		tmp = tmp->next;
		while ( NULL != tmp ){
			if ( tmp->group_id == group_id ){
				return tmp;
			}
			tmp = tmp->next;
		}
	}
	return NULL;
}

/**
 * 重新分配group_list的hash map容量（桶）
 * resize过程是一次性完成的，可以优化成 逐渐完成。
 */
static void terminal_group_list_resize( int new_size ){
	int old_size = group_list_hash_size;
	size_t memory = new_size * sizeof( terminal_group_t * );
	terminal_group_t **old_hash_map = group_hash_map;
	group_hash_map = (terminal_group_t **)malloc( memory );
	if ( NULL == group_hash_map ){
		fprintf(stderr, "terminal_group_list_resize() out of memory\n" );
		return;
	}
	memset( group_hash_map, 0, memory );
	group_list_hash_size = new_size;
	if ( 0 == old_size ){
		return;
	}
	int i;
	for ( i = 0; i < old_size; ++i ){
		if ( NULL == old_hash_map[ i ] ){
			continue;
		}
		terminal_group_t *tmp = old_hash_map[ i ];
		while ( NULL != tmp ){
			int index = tmp->group_id % group_list_hash_size;
			terminal_group_t *group = tmp;
			tmp = tmp->next;
			group->next = group_hash_map[ index ];
			group_hash_map[ index ] = group;
		}
	}
	free( old_hash_map );
}

/**
 * 新建一个组
 */
static terminal_group_t *terminal_group_new( uint32_t group_id ){
	//还没有初始化列表
	if ( 0 == group_list_hash_size ){
		terminal_group_list_resize( DEFAULT_GROUP_HASH_SIZE );
	}
	else if ( group_list_size > group_list_hash_size && group_list_hash_size < MAX_GROUP_HASH_SIZE ){
		int new_size = MAX_GROUP_HASH_SIZE * 2;
		if ( new_size > MAX_GROUP_HASH_SIZE ){
			new_size = MAX_GROUP_HASH_SIZE;
		}
		terminal_group_list_resize( new_size );
	}
	int index = group_id % group_list_hash_size;
	terminal_group_t *group_info = yile_route_group_new();
	if ( NULL == group_info ){
		return NULL;
	}
	group_info->group_id = group_id;
	group_info->next = group_hash_map[ index ];
	group_hash_map[ index ] = group_info;
	++group_list_size;
	return group_info;
}

/**
 * 清除一个group
 */
static void terminal_group_free( terminal_group_t *group_info ){
	if ( 0 == group_list_hash_size ){
		return;
	}
	int index = group_info->group_id % group_list_hash_size;
	if ( NULL == group_hash_map[ index ] ){
		return;
	}
	if ( group_hash_map[ index ]->group_id == group_info->group_id ){
		group_hash_map[ index ] = group_info->next;
		yile_route_group_free( group_info );
	}
	else{
		terminal_group_t *tmp = group_hash_map[ index ]->next;
		terminal_group_t *last = group_hash_map[ index ];
		while ( NULL != tmp ){
			if ( tmp->group_id == group_info->group_id ){
				if ( NULL != last ){
					last->next = group_info->next;
				}
				yile_route_group_free( tmp );
				break;
			}
			last = tmp;
			tmp = tmp->next;
		}
	}
	--group_list_size;
}

/**
 * 将一个终端加入IP组
 */
void terminal_group_add_client( yile_connection_t *fd_info, uint32_t group_id ){
	terminal_group_t *group_info = terminal_group_find( group_id );
	if ( NULL == group_info ){
		group_info = terminal_group_new( group_id );
		if ( NULL == group_info ){
			return;
		}
	}
	terminal_client_t *new_client = terminal_group_client_new();
	if ( NULL == new_client ){
		return;
	}
	new_client->group_info = group_info;
	new_client->fd_info = fd_info;
	new_client->ping_time = time( NULL );
	new_client->last = NULL;
	if ( NULL != group_info->client_list ){
		group_info->client_list->last = new_client;
	}
	new_client->next = group_info->client_list;
	group_info->client_list = new_client;
	group_info->client_size++;
	fd_info->ext_data = new_client;
}

/**
 * 将一个终端移出一个组
 */
void terminal_group_remove_client( yile_connection_t *fd_info ){
	if ( NULL == fd_info->ext_data ){
		return;
	}
	terminal_client_t *client = (terminal_client_t *)fd_info->ext_data;
	terminal_group_t *group_info = client->group_info;
	//双链接从list中移除的常规操作
	if ( NULL == client->last ){
		group_info->client_list = client->next;
	}
	else {
		client->last->next = client->next;
	}
	if ( NULL != client->next ){
		client->next->last = client->last;
	}
	terminal_group_client_free( client );
	--group_info->client_size;
	//没有client了
	if ( 0 == group_info->client_size ){
		terminal_group_free( group_info );
	}
	fd_info->ext_data = NULL;
}

/**
 * 将消息广播给组
 */
void terminal_group_send_msg( uint32_t group_id, yile_buf_t *send_buf ){
	terminal_group_t *group_info = terminal_group_find( group_id );
	if ( NULL == group_info ){
		return;
	}
	terminal_client_t *member = group_info->client_list;
	while ( NULL != member ){
		yile_connection_send( member->fd_info, send_buf->data, send_buf->write_pos );
		member = member->next;
	}
}

/**
 * 踢出不活动的terminal client
 */
int terminal_group_kick_idle( yile_connection_t *timer_fd ){
	//todo
	return YILE_OK;
}