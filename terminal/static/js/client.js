define( function( require, exports, module ){
	var newe = require( 'newe' );
	var $ = require( 'jquery' );
	var url = require( 'url' );
	function websocket_client( host, port, param ){
		this.host = url.build_query_string( param, 'ws://' + host + ':' + port );
		this.websocket = null;
		this.eve_list = {};
	}

	module.exports = websocket_client;
	websocket_client.prototype = {
		read_body: false,
		is_loading: false,
		need_read: 0,
		body_data: '',
		head_str: '',
		open: function(){
			this.websocket = new WebSocket( this.host );
			if ( !this.websocket ){
				this.trigger( 'error', '你的浏览器不支持websocket' );
				return;
			}
			this.websocket.onopen = newe.bind( this.on_open, this );
			this.websocket.onclose = newe.bind( this.on_close, this );
			this.websocket.onmessage = newe.bind( this.on_message, this );
			this.websocket.onerror = newe.bind( this.on_error, this );
		},
		/**
		 * 连接好websocket
		 * @type Boolean|Boolean|Boolean
		 */
		on_open: function(){
			this.trigger( 'open' );
		},
		on: function( eve_name, callback ){
			if ( !newe.isset( this.eve_list[ eve_name ] ) ){
				this.eve_list[ eve_name ] = [];
			}
			this.eve_list[ eve_name ].push( callback );
		},
		/**
		 * 触发事件
		 */
		trigger: function( eve, arg ){
			if ( !newe.isset( this.eve_list[ eve ] ) ){
				return;
			}
			for ( var i = 0; i < this.eve_list[ eve ].length; i++ ){
				var func = this.eve_list[ eve ][ i ];
				if ( 'undefined' === typeof arg ){
					func();
				}
				else {
					func( arg );
				}
			}
		},
		/**
		 * 关闭
		 * @returns {undefined}
		 */
		on_close: function(){
			this.close( true );
			this.trigger( 'close' );
		},
		/**
		 * 收到消息
		 */
		on_message: function( eve ){
			this.trigger( eve.data );
		},
		/**
		 * 出错
		 */
		on_error: function( eve ){
			this.trigger( 'error' );
		},
		/**
		 * 发送数据
		 */
		send: function( action_name, data ){
			if ( 'string' !== typeof action_name ){
				$.error( '无法识别的action name' );
			}
			if ( -1 !== action_name.indexOf( '|' ) ){
				$.error( 'action_name 不能包含“|”字符' );
			}
			if ( 'string' !== typeof data ){
				data = String( data );
			}
			var send_str = action_name + '|' + data;
			console.debug( "send:("+ send_str.length +")" + send_str );
			this.websocket.send( send_str );
		},
		/**
		 * 关闭
		 */
		close: function( is_eve ){
			if ( true !== is_eve && this.websocket ){
				this.websocket.close();
			}
			this.websocket = null;
		}
	};
} );