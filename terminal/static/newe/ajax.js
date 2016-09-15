define( function( require, exports ){
	'use strict';
	var $ = require( 'jquery' );
	var newe = require( 'newe' );
	var newe_win = require( 'window' );
	var ajax_loading = require( 'loading' );
	var is_ajaxing = false;
	var ajax_request_pool = [];

	function ajax_push_pool( ajax_object ){
		if ( is_ajaxing ){
			ajax_request_pool.push( ajax_object );
		}
		else {
			is_ajaxing = true;
			ajax_object.send();
		}
	}

	var has_filter = false, ajax_result_filter = {};

	/**
	 * 检查
	 */
	function ajax_pool_check(){
		if ( 0 === ajax_request_pool.length ){
			is_ajaxing = false;
			return;
		}
		var tmp_obj = ajax_request_pool.shift();
		tmp_obj.send();
	}

	/**
	 * 改良的ajax对象
	 */
	function newe_ajax( url, callback, data, loading_modal, callback_arg ){
		this.ajax_data = data;
		var hostname = get_config( 'hostname', null );
		if ( hostname ){
			url = hostname + url;
		}
		this.url = url;
		this.callback_arg = callback_arg;
		if ( 'function' === typeof callback ){
			this.callback = callback;
		}
		else {
			this.callback = null;
		}
		if ( false === loading_modal ){
			this.loading_modal = false;
		}
		this.event_btn = null;
	}

	newe_ajax.prototype = {
		status: 0,
		loading_modal: true,
		data_type: 'text',
		type: 'GET',
		set_data_type: function( data_type ){
			this.data_type = data_type;
		},
		set_type: function( type ){
			this.type = type;
		},
		send: function(){
			if ( this.level2 ){
				this.xhr.open( 'POST', this.url );
				this.xhr.send( this.ajax_data );
			}
			else {
				var options = {
					url: this.url,
					dataType: this.data_type,
					type: this.type,
					error: newe.bind( this.error_handle, this ),
					success: newe.bind( this.success_handle, this ),
					complete: newe.bind( this.complete_handle, this )
				};
				if ( this.ajax_data && 'POST' === this.type ){
					options.data = this.ajax_data;
				}
				if ( this.loading_modal ){
					ajax_loading.start();
				}
				$.ajax( options );
			}
		},
		complete_handle: function(){
			if ( this.loading_modal ){
				ajax_loading.finish();
			}
			ajax_pool_check();
			//还原btn状态
			if ( this.event_btn ){
				delete( lock_url[ this.url ] );
				$( this.event_btn ).removeClass( 'disabled' );
				if ( 'INPUT' === this.event_btn.nodeName ){
					this.event_btn.value = this.before_text;
				}
				else {
					this.event_btn.innerHTML = this.before_text;
				}
				this.before_text = this.event_btn = null;
			}
		},
		error_handle: function( ajax_obj ){
			console.error( '[AJAX_ERROR]' );
			if ( is_develop() ){
				this.console( '服务器返回', ajax_obj.responseText );
				this.dump_console();
				newe.body.trigger( 'ajax.error', ajax_obj.responseText ); 
			}
		},
		success_handle: function( data ){
			var is_dev = is_develop();
			if ( 'json' === this.data_type ){
				var name, success;
				//过滤器执行
				if ( has_filter ){
					for ( name in ajax_result_filter ){
						success = ajax_result_filter[ name ]( data, this );
						if ( is_dev ){
							this.console( '执行过滤器' + name, success ? '成功' : '失败，程序中止' );
						}
						if ( !success ){
							return;
						}
					}
				}
				if ( is_dev ){
					this.console( '服务器返回', print_r( data ) );
				}
				//数据修正
				if ( this.fix_data ){
					fix_json( data, this.fix_data );
				}
			}
			else if ( is_dev ){
				console.debug( '返回结果', data );
			}
			if ( this.callback ){
				try{
					this.callback( data, this.callback_arg );
				}
				catch ( err ){
					console.error( 'Ajax 请求回调函数执行出错' );
					console.error( err );
					console.error( err.message );
					console.debug( this );
				}
			}
			if ( is_dev ){
				this.dump_console();
			}
		},
		console: function( title, data ){
			if ( !this.console_array ){
				this.console_array = [];
				if ( 'POST' === this.type ){
					this.console_array.push( [ 'post data', newe_var.clone( this.ajax_data ) ] );
				}
			}
			this.console_array.push( [ title, 'string' !== typeof data ? newe_var.clone( data ) : data ] );
		},
		dump_console: function(){
			if ( !this.console_array ){
				return;
			}
			console.groupCollapsed( '[AJAX ' + this.type + ']' + this.url );
			var tmp_arr;
			for ( var i = 0, len = this.console_array.length; i < len; ++i ){
				tmp_arr = this.console_array[ i ];
				console.info( tmp_arr[ 0 ], tmp_arr[ 1 ] );
			}
			console.groupEnd();
		},
		/**
		 * 锁定通过用户操作而触发的ajax请求的按钮
		 */
		lock: function( eve, loading_text ){
			lock_url[ this.url ] = true;
			loading_text = loading_text || '加载中';
			eve = eve || newe.searchEvent();
			var target = eve.target;
			if ( !target || 1 !== target.nodeType ){
				return this;
			}
			if ( 'INPUT' === target.nodeName ){
				this.before_text = target.value;
				target.value = loading_text;
			}
			else {
				this.before_text = target.innerHTML;
				target.innerHTML = loading_text;
			}
			this.event_btn = target;
			$( target ).addClass( 'disabled' );
			return this;
		},
		/**
		 * 数据修正
		 */
		fix: function( fix_data ){
			if ( 'object' !== fix_data || empty( fix_data ) || $.isArray( fix_data ) ){
				return;
			}
			this.fix_data = fix_data;
		}
	};
	//被锁定的url
	var lock_url = {};

	/**
	 * 判断url是否正在加载中，锁定
	 */
	function is_lock_url( url ){
		var re = lock_url[ url ];
		if ( re ){
			console.debug( '[AJAX LOCK]' + url );
		}
		return re;
	}

	/**
	 * 返回一个空的ajax对象。为了开发方便，不用每次都检测ajax返回的结果是否正确
	 */
	function empty_ajax_object(){
		return {
			status: -1,
			lock: newe.empty
		};
	}

	exports.get = function( url, callback, loading_modal, callback_arg ){
		if ( is_lock_url( url ) ){
			return empty_ajax_object();
		}
		var tmp_req = new newe_ajax( url, callback, null, loading_modal, callback_arg );
		tmp_req.set_data_type( 'json' );
		ajax_push_pool( tmp_req );
		return tmp_req;
	};
	exports.post = function( url, data, callback, loading_modal, callback_arg ){
		if ( is_lock_url( url ) ){
			return empty_ajax_object();
		}
		var tmp_req = new newe_ajax( url, callback, data, loading_modal, callback_arg );
		tmp_req.set_data_type( 'json' );
		tmp_req.set_type( 'POST' );
		ajax_push_pool( tmp_req );
		return tmp_req;
	};
	exports.get_text = function( url, callback, loading_modal, callback_arg ){
		if ( is_lock_url( url ) ){
			return empty_ajax_object();
		}
		var tmp_req = new newe_ajax( url, callback, null, loading_modal, callback_arg );
		ajax_push_pool( tmp_req );
		return tmp_req;
	};
	exports.post_text = function( url, data, callback, loading_modal, callback_arg ){
		if ( is_lock_url( url ) ){
			return empty_ajax_object();
		}
		var tmp_req = new newe_ajax( url, callback, data, loading_modal, callback_arg );
		tmp_req.set_type( 'POST' );
		ajax_push_pool( tmp_req );
		return tmp_req;
	};
	/**
	 * 增加全局查询结果过滤器
	 */
	exports.add_filter = function( name, handle ){
		if ( 'string' !== typeof name || name.length < 1 || 'function' !== typeof handle ){
			$.error( 'ajax add filter failed' );
		}
		ajax_result_filter[ name ] = handle;
		has_filter = true;
	}
} )
;