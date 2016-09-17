#include "proto.h"

/**
 * 生成 加入结果
 */
void write_join_result( yile_buf_t *pack_result, proto_join_result_t *data_arr )
{
	pack_result->write_pos = 0;
	packet_head_t packet_info;
	packet_info.pack_id = 102;
	yile_buf_write( pack_result, NULL, sizeof( packet_head_t ) );
	yile_buf_write( pack_result, &data_arr->status, sizeof( data_arr->status ) );
	yile_protocol_write_string( pack_result, data_arr->message );
	packet_info.size = pack_result->write_pos - sizeof( packet_head_t );
	memcpy( pack_result->data, &packet_info, sizeof( packet_head_t ) );
}

/**
 * 生成 ping结果
 */
void write_pong( yile_buf_t *pack_result, proto_pong_t *data_arr )
{
	pack_result->write_pos = 0;
	packet_head_t packet_info;
	packet_info.pack_id = 104;
	yile_buf_write( pack_result, NULL, sizeof( packet_head_t ) );
	yile_buf_write( pack_result, &data_arr->status, sizeof( data_arr->status ) );
	yile_protocol_write_string( pack_result, data_arr->message );
	packet_info.size = pack_result->write_pos - sizeof( packet_head_t );
	memcpy( pack_result->data, &packet_info, sizeof( packet_head_t ) );
}

/**
 * 解析 加入服务器
 */
proto_join_xtail_t *read_join_xtail( yile_buf_t *byte_pack, protocol_pool_t *result_pool )
{
	proto_join_xtail_t *re_struct = NULL;
	uint32_t check_code = yile_protocol_read_uint( byte_pack );
	uint32_t protocol_code = yile_protocol_check( (unsigned char*)&byte_pack->data[ byte_pack->read_pos ], byte_pack->write_pos - byte_pack->read_pos, (unsigned char*)"2c1e6f9ee0486bb26e06f3c749582ac1", 32 );
	if( check_code != protocol_code )
	{
		result_pool->error_code = PROTO_CHECK_CODE_ERROR;
		return NULL;
	}
	if ( NULL == re_struct )
	{
		re_struct = ( proto_join_xtail_t* )yile_protocol_malloc( result_pool, sizeof( proto_join_xtail_t ) );
		if ( NULL == re_struct )
		{
			return re_struct;
		}
	}
	re_struct->auth_str = yile_protocol_read_string( byte_pack, result_pool );
	if ( byte_pack->error_code || result_pool->error_code )
	{
		return NULL;
	}
	return re_struct;
}

/**
 * 解析 ping
 */
proto_ping_t *read_ping( yile_buf_t *byte_pack, protocol_pool_t *result_pool )
{
	proto_ping_t *re_struct = NULL;
	if ( NULL == re_struct )
	{
		re_struct = ( proto_ping_t* )yile_protocol_malloc( result_pool, sizeof( proto_ping_t ) );
		if ( NULL == re_struct )
		{
			return re_struct;
		}
	}
	re_struct->time = yile_protocol_read_uint( byte_pack );
	if ( byte_pack->error_code || result_pool->error_code )
	{
		return NULL;
	}
	return re_struct;
}

/**
 * 解析 推送数据
 */
proto_push_msg_t *read_push_msg( yile_buf_t *byte_pack, protocol_pool_t *result_pool )
{
	proto_push_msg_t *re_struct = NULL;
	if ( NULL == re_struct )
	{
		re_struct = ( proto_push_msg_t* )yile_protocol_malloc( result_pool, sizeof( proto_push_msg_t ) );
		if ( NULL == re_struct )
		{
			return re_struct;
		}
	}
	re_struct->group_id = yile_protocol_read_uint( byte_pack );
	re_struct->msg_type = yile_protocol_read_byte( byte_pack );
	re_struct->host_name = yile_protocol_read_string( byte_pack, result_pool );
	re_struct->msg = yile_protocol_read_bin( byte_pack, NULL, result_pool );
	if ( byte_pack->error_code || result_pool->error_code )
	{
		return NULL;
	}
	return re_struct;
}
/**
 * 计算解析内存 加入服务器
 */
uint32_t size_read_join_xtail( yile_buf_t *byte_pack )
{
	uint32_t old_byte_pack_size = byte_pack->read_pos;
	protocol_pool_t unpack_pool = {0};
	protocol_pool_t *result_pool = &unpack_pool;
	yile_protocol_read_uint( byte_pack );
	yile_protocol_premalloc( result_pool, sizeof( proto_join_xtail_t ) );
	yile_protocol_str_len_t tmp_str_len;
	tmp_str_len = yile_protocol_read_ushort( byte_pack );
	yile_protocol_size_check( byte_pack, tmp_str_len );
	yile_protocol_premalloc( result_pool, tmp_str_len + 1 );
	byte_pack->read_pos = old_byte_pack_size;
	if ( result_pool->error_code || byte_pack->error_code )
	{
		return 0;
	}
	return result_pool->max_size;
}
/**
 * 计算解析内存 推送数据
 */
uint32_t size_read_push_msg( yile_buf_t *byte_pack )
{
	uint32_t old_byte_pack_size = byte_pack->read_pos;
	protocol_pool_t unpack_pool = {0};
	protocol_pool_t *result_pool = &unpack_pool;
	yile_protocol_premalloc( result_pool, sizeof( proto_push_msg_t ) );
	yile_protocol_size_check( byte_pack, sizeof( uint32_t ) );
	yile_protocol_size_check( byte_pack, sizeof( int8_t ) );
	yile_protocol_str_len_t tmp_str_len;
	tmp_str_len = yile_protocol_read_ushort( byte_pack );
	yile_protocol_size_check( byte_pack, tmp_str_len );
	yile_protocol_premalloc( result_pool, tmp_str_len + 1 );
	yile_protocol_byte_len_t tmp_bin_len;
	yile_protocol_premalloc( result_pool, sizeof( yile_protocol_byte_t ) );
	tmp_bin_len = yile_protocol_read_uint( byte_pack );
	yile_protocol_size_check( byte_pack, tmp_bin_len );
	yile_protocol_premalloc( result_pool, tmp_bin_len );
	byte_pack->read_pos = old_byte_pack_size;
	if ( result_pool->error_code || byte_pack->error_code )
	{
		return 0;
	}
	return result_pool->max_size;
}
#ifdef PROTOCOL_DEBUG

/**
 * 打印 加入服务器
 */
void print_join_xtail( proto_join_xtail_t *re )
{
	int rank = 0;
	char prefix_char[ MAX_LIST_RECURSION * 4 + 1 ];
	yile_printf_tab_string( prefix_char, rank );
	printf( "join_xtail\n" );
	printf( "%s(\n", prefix_char );
	printf( "    %s[auth_str] = > ", prefix_char );
	printf( "%s\n", re->auth_str );
	printf( "%s)\n", prefix_char );
	}

/**
 * 打印 ping
 */
void print_ping( proto_ping_t *re )
{
	int rank = 0;
	char prefix_char[ MAX_LIST_RECURSION * 4 + 1 ];
	yile_printf_tab_string( prefix_char, rank );
	printf( "ping\n" );
	printf( "%s(\n", prefix_char );
	printf( "    %s[time] = > ", prefix_char );
	printf( "%u\n", re->time );
	printf( "%s)\n", prefix_char );
	}

/**
 * 打印 推送数据
 */
void print_push_msg( proto_push_msg_t *re )
{
	int rank = 0;
	char prefix_char[ MAX_LIST_RECURSION * 4 + 1 ];
	yile_printf_tab_string( prefix_char, rank );
	printf( "push_msg\n" );
	printf( "%s(\n", prefix_char );
	printf( "    %s[host_name] = > ", prefix_char );
	printf( "%s\n", re->host_name );
	printf( "    %s[group_id] = > ", prefix_char );
	printf( "%u\n", re->group_id );
	printf( "    %s[msg_type] = > ", prefix_char );
	printf( "%d\n", re->msg_type );
	printf( "    %s[msg] = > ", prefix_char );
	printf( "[Blob %d]\n", re->msg->len );
	printf( "%s)\n", prefix_char );
	}
#endif
