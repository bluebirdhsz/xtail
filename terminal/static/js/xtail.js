define( function( require ){
	var $ = require( 'jquery' );
	var url = require( 'url' );
	var websocket_client = require( 'xtail/client' );
	var ip = url.parse_query( '', 'ip' ), param = {};
	set_config( 'is_debug', 1 == url.parse_query( '', 'is_debug' ) );
	
	if ( ip ){
		param.ip = ip;
	}
	function debug(){
		is_develop() && console.info.apply( console, Array.prototype.concat.apply( [ '[Websocket]' ], arguments ) );
	}
	function xtail_head( msg, css ){
		var head_obj = $( '#xtail_head_div' ).html( msg );
		head_obj[ 0 ].className = css;
	}
	var host = get_config( 'host', '127.0.0.1' );
	var port = get_config( 'port', '8080' );
	debug( 'connect to ws://' + host + ':' + port, param );
	var client = new websocket_client( host, port, param );
	client.on( 'open', function(){
		debug( 'connect success' );
		client.send( 'ping', 'ping' );
	} );
	client.on( 'error', function(){
		xtail_head( '无法加入服务器', 'error' );
	} );
	client.open();
} );