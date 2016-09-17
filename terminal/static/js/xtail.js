define( function( require ){
	var $ = require( 'jquery' );
	var config = require( 'xtail/config' );
	var url = require( 'url' );
	var websocket_client = require( 'xtail/client' );
	var ip = url.parse_query( '', 'ip' ), param = {};
	if ( ip ){
		param.ip = ip;
	}
	var client = new websocket_client( config.host, config.port, param );
	client.on( 'open', function(){
		console.debug( 'connect success' );
		client.send( 'ping', 'ping' );
	} );
	client.open();
	console.debug( 'run' );
} );