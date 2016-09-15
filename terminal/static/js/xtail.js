define( function( require ){
	var $ = require( 'jquery' );
	var config = require( 'xtail/config' );
	var websocket_client = require( 'xtail/client' );
	var client = new websocket_client( config.host, config.port );
	client.on( 'open', function(){
		console.debug( 'connect success' );
	} );
	client.open();
	console.debug( 'run' );
} );