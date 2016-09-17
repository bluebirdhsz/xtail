<?php

/**
 * 打包数据 加入服务器
 */
function php_pack_join_xtail( $data )
{
	$bin_str = '';
	$vauth_str_len = strlen( $data[ "auth_str" ] );
	$bin_str .= pack( 'Sa'.$vauth_str_len.'', $vauth_str_len, $data[ "auth_str" ] );
	$head_str = pack( "LS", strlen( $bin_str ) + 4, 101 );
	$check_code = yile_protocol_check( $bin_str, "2c1e6f9ee0486bb26e06f3c749582ac1" );
	$head_str .= pack( "L", $check_code );
	$bin_str = $head_str . $bin_str;
	return $bin_str;
}
/**
 * 打包数据 ping
 */
function php_pack_ping( $data )
{
	$bin_str = '';
	$bin_str .= pack( 'L', $data[ "time" ] );
	$head_str = pack( "LS", strlen( $bin_str ), 103 );
	$bin_str = $head_str . $bin_str;
	return $bin_str;
}
/**
 * 打包数据 推送数据
 */
function php_pack_push_msg( $data )
{
	$bin_str = '';
	$vfile_name_len = strlen( $data[ "file_name" ] );
	$vmsg_len = strlen( $data[ "msg" ] );
	$bin_str .= pack( 'LcSa'.$vfile_name_len.'Sa'.$vmsg_len.'', $data[ "ip" ], $data[ "msg_type" ], $vfile_name_len, $data[ "file_name" ], $vmsg_len, $data[ "msg" ] );
	$head_str = pack( "LS", strlen( $bin_str ), 103 );
	$bin_str = $head_str . $bin_str;
	return $bin_str;
}

/**
 * 解包数据 加入结果
 */
function php_unpack_join_result( $bin_str )
{
	$unpack_pos = 0;
	$result = unpack( "Lstatus/", substr( $bin_str, $unpack_pos ) );
	$unpack_pos = 4;
	$result[ "message" ] = yile_protocol_unpack_str( $bin_str, $unpack_pos, 2 );
	return $result;
}
/**
 * 解包数据 ping结果
 */
function php_unpack_pong( $bin_str )
{
	$unpack_pos = 0;
	$result = unpack( "Lstatus/", substr( $bin_str, $unpack_pos ) );
	$unpack_pos = 4;
	$result[ "message" ] = yile_protocol_unpack_str( $bin_str, $unpack_pos, 2 );
	return $result;
}
if ( !function_exists( "yile_protocol_unpack_str" ) )
{
	function yile_protocol_unpack_str( $bin_str, &$uppack_pos, $len_byte = 2 )
	{
		$len_str = substr( $bin_str, $uppack_pos, $len_byte );
		$uppack_pos += $len_byte;
		$tmp_str = 2 == $len_byte ? "Slen" : "Llen";
		$tmp = unpack( $tmp_str, $len_str );
		if ( 0 == $tmp[ "len" ] )
		{
			$str = "";
		}
		else
		{
			$str = substr( $bin_str, $uppack_pos, $tmp[ "len" ] );
			$uppack_pos += $tmp[ "len" ];
		}
		return $str;
	}
}
if ( !function_exists( "yile_protocol_check" ) )
{
	function yile_protocol_check( $str, $private_key )
	{
		$mod_adler = 65521;
		$data_len = strlen( $str );
		$a = 1;
		$b = 0;
		for ( $i = 0; $i < $data_len; ++$i )
		{
			$tmp = ord( $str{ $i } );
			$a = ( $a + $tmp ) % $mod_adler;
			$b = ( $b + $a ) % $mod_adler;
		}
		$key_len = strlen( $private_key );
		for ( $m = 0; $m < $key_len; ++$m )
		{
			$tmp = ord( $private_key{ $m } );
			$a = ( $a + $tmp ) % $mod_adler;
			$b = ( $b + $a ) % $mod_adler;
		}
		return ( $b << 16 ) | $a;
	}
}
