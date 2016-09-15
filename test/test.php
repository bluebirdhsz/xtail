<?php
define( 'ROOT_PATH', dirname( dirname( __FILE__ ) ) . DIRECTORY_SEPARATOR );
/** @noinspection PhpIncludeInspection */
require_once ROOT_PATH .'protocol/build/php/proto.php';
$fd = stream_socket_client( 'tcp://127.0.0.1:8080' );
if ( !$fd ){
	echo "can not join xtail server\n";
	return;
}
$data = array(
	'auth_str' => 'www.ffan.com'
);
$send_data = php_pack_join_xtail( $data );
fwrite( $fd, $send_data );
sleep( 1 );
while( true )
{
	$read_re = fread( $fd, 25000 );
	if( $read_re )
	{
		break;
	}
}
echo "read:". $read_re ."\n";