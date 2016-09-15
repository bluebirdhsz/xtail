define( function( require, exports, module ){
	var newe = require( 'newe' );

	function websocket_client( host, port ){
		this.host = 'ws://' + host + ':' + port;
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
			console.debug( "open" );
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
			console.warn( '连接断开' );
			this.trigger( 'close' );
		},
		/**
		 * 收到消息
		 */
		on_message: function( eve ){
			this.read_data( eve.data );
		},
		/**
		 * 出错
		 */
		on_error: function( eve ){
			console.error( 'websocket 出错了' );
			console.error( eve );
			this.trigger( 'error' );
		},
		/**
		 * 收到消息
		 */
		read_data: function( data ){
			//读数据体
			if ( this.read_body ){
				this.body_data += data;
				var data_len = str_utf8_len( data );
				if ( data_len === this.need_read ){
					this.read_body = false;
					this.trigger( 'data', { head: this.head_str, body: this.body_data } );
				}
				else {
					this.need_read -= data_len;
				}
			}
			//读数据头
			else {
				this.body_data = '';
				var pos = data.indexOf( '|' );
				if ( -1 === pos ){
					this.close();
				}
				this.head_str = data.substring( 0, pos );
				data = data.substring( pos + 1 );
				pos = data.indexOf( '|' );
				if ( -1 === pos ){
					this.close();
				}
				var size = parseInt( data.substring( 0, pos ) );
				data = data.substring( pos + 1 );
				this.need_read = size;
				this.read_body = true;
				this.read_data( data );
			}
		},
		/**
		 * 发送数据
		 */
		send: function( act, data ){
			var send_str = '|' + act + '|' + data;
			this.websocket.send( str_utf8_len( send_str ) + send_str );
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
	/**
	 * utf8字符串长度
	 */
	function str_utf8_len( string ){
		var str = '' + string;
		var total_len = 0, i;
		var charCode, len = str.length;
		for ( i = 0; i < len; i++ ){
			charCode = str.charCodeAt( i );
			if ( charCode < 0x007f ){
				total_len++;
			}
			else if ( charCode <= 0x07ff ){
				total_len += 2;
			}
			else if ( charCode <= 0xffff ){
				total_len += 3;
			}
		}
		return total_len;
	}
} );