#include "proto_so.h"
/**
 * 打包 加入服务器
 */
void sowrite_join_xtail( yile_buf_t *pack_result, HashTable *data_hash )
{
	zval **tmp_data;
	pack_result->write_pos = 0;
	packet_head_t packet_info;
	packet_info.size = 0;
	packet_info.pack_id = 101;
	yile_buf_write( pack_result, NULL, PROTO_CHECK_CODE_HEAD );
	read_string_from_hash( proto_so_join_xtail, auth_str );
	uint32_t check_code = yile_protocol_check( &pack_result->data[ PROTO_CHECK_CODE_HEAD ], pack_result->write_pos - PROTO_CHECK_CODE_HEAD, "2c1e6f9ee0486bb26e06f3c749582ac1", 32 );
	memcpy( pack_result->data + sizeof( packet_head_t ), &check_code, sizeof( check_code ) );
	packet_info.size = pack_result->write_pos - sizeof( packet_head_t );
	memcpy( pack_result->data, &packet_info, sizeof( packet_head_t ) );
}
/**
 * 打包 ping
 */
void sowrite_ping( yile_buf_t *pack_result, HashTable *data_hash )
{
	zval **tmp_data;
	pack_result->write_pos = 0;
	packet_head_t packet_info;
	packet_info.size = 0;
	packet_info.pack_id = 103;
	yile_buf_write( pack_result, NULL, sizeof( packet_head_t ) );
	proto_so_ping_t proto_so_ping;
	read_int_from_hash( proto_so_ping, time );
	yile_buf_write( pack_result, &proto_so_ping, sizeof( proto_so_ping_t ) );
	packet_info.size = pack_result->write_pos - sizeof( packet_head_t );
	memcpy( pack_result->data, &packet_info, sizeof( packet_head_t ) );
}
/**
 * 打包 推送数据
 */
void sowrite_push_msg( yile_buf_t *pack_result, HashTable *data_hash )
{
	zval **tmp_data;
	pack_result->write_pos = 0;
	packet_head_t packet_info;
	packet_info.size = 0;
	packet_info.pack_id = 103;
	yile_buf_write( pack_result, NULL, sizeof( packet_head_t ) );
	uint32_t tmp_var_uint32_t;
	read_int_from_hash_var( proto_so_push_msg, tmp_var_uint32_t, ip );
	yile_buf_write( pack_result, &tmp_var_uint32_t, sizeof( tmp_var_uint32_t ) );
	int8_t tmp_var_int8_t;
	read_int_from_hash_var( proto_so_push_msg, tmp_var_int8_t, msg_type );
	yile_buf_write( pack_result, &tmp_var_int8_t, sizeof( tmp_var_int8_t ) );
	read_string_from_hash( proto_so_push_msg, file_name );
	read_string_from_hash( proto_so_push_msg, msg );
	packet_info.size = pack_result->write_pos - sizeof( packet_head_t );
	memcpy( pack_result->data, &packet_info, sizeof( packet_head_t ) );
}
/** 解析 加入结果
 */
void soread_join_result( yile_buf_t *byte_pack, zval *result_arr )
{
	uint32_t tmp_var_uint32_t;
	tmp_var_uint32_t = yile_protocol_read_uint( byte_pack );
	add_assoc_long( result_arr, "status", tmp_var_uint32_t );
	yile_protocol_str_len_t tmp_str_len;
	tmp_str_len = yile_protocol_read_ushort( byte_pack );
	if ( yile_buf_read_available( byte_pack ) >= tmp_str_len )
	{
		add_assoc_stringl( result_arr, "message", &byte_pack->data[ byte_pack->read_pos ], tmp_str_len, 1 );
	}
	yile_protocol_size_check( byte_pack, tmp_str_len );
}
/** 解析 ping结果
 */
void soread_pong( yile_buf_t *byte_pack, zval *result_arr )
{
	uint32_t tmp_var_uint32_t;
	tmp_var_uint32_t = yile_protocol_read_uint( byte_pack );
	add_assoc_long( result_arr, "status", tmp_var_uint32_t );
	yile_protocol_str_len_t tmp_str_len;
	tmp_str_len = yile_protocol_read_ushort( byte_pack );
	if ( yile_buf_read_available( byte_pack ) >= tmp_str_len )
	{
		add_assoc_stringl( result_arr, "message", &byte_pack->data[ byte_pack->read_pos ], tmp_str_len, 1 );
	}
	yile_protocol_size_check( byte_pack, tmp_str_len );
}
