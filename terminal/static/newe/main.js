'use strict';
/**
 * 将数组里的指定值移除掉
 */
function newe_array_unset( arr, val ){
	var new_arr = [], i;
	if ( 'function' === typeof val ){
		for ( i = 0; i < arr.length; i++ ){
			if ( val( arr[ i ] ) ){
				continue;
			}
			new_arr.push( arr[ i ] );
		}
	}
	else {
		for ( i = 0; i < arr.length; i++ ){
			if ( arr[ i ] == val ){
				continue;
			}
			new_arr.push( arr[ i ] );
		}
	}
	return new_arr;
}
/**
 * 移除数组里的一个index
 */
function newe_array_remove_index( arr, index ){
	var new_arr = [];
	for ( var i = 0; i < arr.length; ++i ){
		if ( i == index ){
			continue;
		}
		new_arr.push( arr[ i ] );
	}
	return new_arr;
}
/**
 * 对象合并，返回新对象
 */
function newe_object_merge( object_to, object_from ){
	var result = {};
	if ( 'object' !== typeof object_from || 'object' !== typeof object_to ){
		return;
	}
	for ( var p in object_to ){
		result[ p ] = object_to[ p ];
	}
	for ( p in object_from ){
		result[ p ] = object_from[ p ];
	}
	return result;
}
/**
 * 对象合并，直接替换原来的对象
 */
function newe_object_append( object_to, object_from ){
	if ( 'object' !== typeof object_from ){
		return;
	}
	for ( var p in object_from ){
		object_to[ p ] = object_from[ p ];
	}
}
/**
 * 深拷贝变量
 */
var newe_var = (function(){
	function gtype( o ){
		if ( 'object' === typeof o ){
			if ( null === o ){
				return 0;
			}
			if ( Array.isArray( o ) || toString.call( o ) === '[object Array]' ){
				return 1;
			}
			return 2;
		}
		else {
			return 0;
		}
	}

	function deep_clone( source, tp ){
		var destination;
		if ( 0 === tp ){
			return source;
		}
		else {
			destination = 1 === tp ? [] : {};
			for ( var p in source ){
				var tmp_tp = gtype( source[ p ] );
				if ( tmp_tp > 0 ){
					destination[ p ] = deep_clone( source[ p ] );
				}
				else {
					destination[ p ] = source[ p ];
				}
			}
		}
		return destination;
	}

	function merge( aim_object, source ){
		var p, aim_tp, source_tp;
		for ( p in source ){
			source_tp = gtype( source[ p ] );
			if ( !isset( aim_object[ p ] ) ){
				//普通类型，直接附加
				if ( 0 === source_tp ){
					aim_object[ p ] = source[ p ];
				}
				//数组和对象，深拷贝
				else {
					aim_object[ p ] = deep_clone( source[ p ], source_tp );
				}
			}
			else {
				aim_tp = gtype( aim_object[ p ] );
				//如果类型不一样，直接替换
				if ( aim_tp !== source_tp ){
					aim_object[ p ] = 0 === source_tp ? source[ p ] : deep_clone( source[ p ], source_tp );
				}
				else {
					//如果都是普通类型
					if ( 0 === aim_tp ){
						aim_object[ p ] = source[ p ];
					}
					else {
						merge( aim_object[ p ], source[ p ] );
					}
				}
			}
		}
	}

	return {
		clone: function( source ){
			return deep_clone( source, gtype( source ) );
		},
		merge: function( aim_object, source ){
			var aim_tp = gtype( aim_object ), source_tp = gtype( source );
			if ( 0 == aim_tp || 0 == source_tp ){
				throw Error( 'aim_object or source is not object and array' );
			}
			merge( aim_object, source );
			return aim_object;
		}
	}
})();
/**
 * 复制数组
 */
function newe_copy_array( arr ){
	var result = [];
	if ( 'object' !== typeof arr || !arr.length ){
		return result;
	}
	for ( var i = 0; i < arr.length; i++ ){
		result.push( arr[ i ] );
	}
	return result;
}
/**
 * 和php的htmlspecialchars_decode的功能一样
 */
function htmlspecialchars_decode( string, quote_style ){
	var optTemp = 0, i = 0, noquotes = false;
	if ( 'undefined' == typeof quote_style ){
		quote_style = 2;
	}
	string = string.toString().replace( /&lt;/g, '<' ).replace( /&gt;/g, '>' );
	var OPTS = {
		'ENT_NOQUOTES': 0,
		'ENT_HTML_QUOTE_SINGLE': 1,
		'ENT_HTML_QUOTE_DOUBLE': 2,
		'ENT_COMPAT': 2,
		'ENT_QUOTES': 3,
		'ENT_IGNORE': 4
	};
	if ( 0 === quote_style ){
		noquotes = true;
	}
	if ( 'number' !== typeof quote_style ){
		quote_style = [].concat( quote_style );
		for ( i = 0; i < quote_style.length; i++ ){
			if ( 0 === OPTS[ quote_style[ i ] ] ){
				noquotes = true;
			}
			else if ( OPTS[ quote_style[ i ] ] ){
				optTemp = optTemp | OPTS[ quote_style[ i ] ];
			}
		}
		quote_style = optTemp;
	}
	if ( ( quote_style & OPTS.ENT_HTML_QUOTE_SINGLE ) ){
		string = string.replace( /&#0*39;/g, "'" );
	}
	if ( !noquotes ){
		string = string.replace( /&quot;/g, '"' );
	}
	string = string.replace( /&amp;/g, '&' ).replace( /<script/gi, '<scirpt' ).replace( /onerror=/gi, 'onerorr=' );
	return string;
}
if ( !String.prototype.trim ){
	String.prototype.trim = function(){
		return this.replace( /(^\s*)|(\s*$)/g, '' );
	};
}
/**
 * 将字符串变成数组
 */
function newe_str_to_array( str, main_flag, sub_flag ){
	main_flag = main_flag || ',';
	sub_flag = sub_flag || ':';
	var result = {};
	if ( 'string' !== typeof str ){
		return result;
	}
	var main_arr = str.split( main_flag );
	for ( var i = 0; i < main_arr.length; i++ ){
		var tmp = main_arr[ i ].split( sub_flag );
		if ( 2 === tmp.length ){
			result[ tmp[ 0 ].trim() ] = tmp[ 1 ].trim();
		}
	}
	return result;
}
/**
 * 将数组变成字符串
 */
function newe_array_to_str( arr, main_flag, sub_flag ){
	main_flag = main_flag || ',';
	sub_flag = sub_flag || ':';
	var result = [];
	if ( 'object' !== typeof arr ){
		return '';
	}
	for ( var p in arr ){
		result.push( p + sub_flag + String( arr[ p ] ) );
	}
	return result.join( main_flag );
}
/**
 * 去除html
 */
function strip_tags( input, allowed ){
	allowed = ( ( ( allowed || '' ) + '' )
	.toLowerCase()
	.match( /<[a-z][a-z0-9]*>/g ) || [] )
	.join( '' );
	var tags = /<\/?([a-z][a-z0-9]*)\b[^>]*>/gi, commentsAndPhpTags = /<!--[\s\S]*?-->|<\?(?:php)?[\s\S]*?\?>/gi;
	return input.replace( commentsAndPhpTags, '' ).replace( tags, function( $0, $1 ){
		return allowed.indexOf( '<' + $1.toLowerCase() + '>' ) > -1 ? $0 : '';
	} );
}
/**
 * 产生一个随机数
 */
function newe_mt_rand( min, max ){
	min = parseInt( min, 10 );
	max = parseInt( max, 10 );
	return Math.floor( Math.random() * ( max - min + 1 ) ) + min;
}
/**
 * 一个简单的模型管理器（临时的）
 */
var model_manager = (function(){
	var model_arr = {};
	var event_arr = {};
	var alias_arr = {};
	var alias_event = {};

	/**
	 * alias回调
	 */
	function alisa_call( name, model ){
		if ( !isset( alias_event[ name ] ) ){
			return;
		}
		var tp = typeof alias_event[ name ];
		if ( isset( alias_event[ name ] ) ){
			run_callback( alias_event[ name ], model );
		}
	}

	/**
	 * 调试
	 */
	function debug(){
		if ( !is_develop() ){
			return;
		}
		var arg = Array.prototype.concat.apply( [ '[MODEL]' ], arguments );
		var model = arg.pop();
		console.groupCollapsed( arg.join( ' ' ) );
		console.info( print_r( model ) );
		console.groupEnd();
	}

	/**
	 * 更新model
	 */
	function set_model( name, model, no_event ){
		debug( 'set', name, print_r( model ) );
		// 有.的情况
		if ( -1 !== name.indexOf( '.' ) ){
			var find_path = name.split( '.' );
			var root_data = model_arr, i, len = find_path.length - 1, tmp;
			for ( i = 0; i < len; ++i ){
				tmp = find_path[ i ];
				if ( 0 == tmp.length ){
					continue;
				}
				//如果没有这个key，强加，暂时这样先
				if ( 'object' !== typeof root_data[ tmp ] && null !== root_data[ tmp ] ){
					root_data[ tmp ] = {};
				}
				root_data = root_data[ tmp ];
			}
			tmp = find_path[ len ];
			root_data[ tmp ] = model;
			if ( true === no_event ){
				return;
			}
			//事件判断
			var event_name = '';
			root_data = model_arr;
			len = find_path.length;
			for ( i = 0; i < len; i++ ){
				tmp = find_path[ i ];
				if ( 0 == tmp.length ){
					continue;
				}
				if ( i > 0 ){
					event_name += '.';
				}
				event_name += tmp;
				if ( isset( event_arr[ event_name ] ) ){
					run_callback( event_arr[ event_name ], root_data[ tmp ] );
				}
				root_data = root_data[ tmp ];
			}
		}
		else {
			model_arr[ name ] = model;
			if ( !no_event && isset( event_arr[ name ] ) ){
				run_callback( event_arr[ name ], model );
			}
		}
	}

	/**
	 * callback检查
	 */
	function callback_check( callback, tpl_name ){
		var tp = typeof callback;
		if ( 'function' === tp ){
			return callback;
		}
		if ( 'string' === tp && callback.length > 0 ){
			if ( 'string' === typeof tpl_name && tpl_name.length > 0 ){
				callback += ':' + tpl_name;
			}
			return callback;
		}
		console.error( tp, callback );
		$.error( 'modelmanager callback error' );
	}

	/**
	 * 运行callback
	 */
	function run_callback( callback, model ){
		if ( 'function' === typeof callback ){
			callback( model );
		}
		//中间有 : 号，表示 container 和 模板
		else if ( -1 !== callback.indexOf( ':' ) ){
			if ( 'function' !== typeof window.newe_smarty ){
				console.error( '请先加载smarty.js' );
				return;
			}
			var tmp = callback.split( ':' );
			window.newe_smarty( tmp[ 1 ], model, tmp[ 0 ] );
		}
		else {
			$( '#' + callback ).html( String( model ) );
		}
	}

	return {
		/**
		 * 设置一个模型
		 * @param {string} name
		 * @param {*} model 数据
		 */
		set: function( name, model ){
			var no_event = false, real_name = name;
			//通过别名去更新，不会触发其它事件，只会触发别名事件
			if ( 'string' === typeof alias_arr[ name ] ){
				real_name = alias_arr[ name ];
				no_event = true;
			}
			set_model( real_name, model, no_event );
			if ( no_event ){
				alisa_call( name, model );
			}
		},
		/**
		 * 将一个模型和原始模型合并
		 */
		merge: function( name, data ){
			//如果传入的数据不是对象或者数组，将直接set
			if ( 'object' !== typeof data && null !== data ){
				this.set( name, data );
				return;
			}
			var aim_model = this.get( name, {} );
			var new_data = newe_var.merge( aim_model, data );
			this.set( name, new_data );
		},
		/**
		 * 设置一个关联数组model的其中一项
		 */
		set_item: function( name, index, model ){
			if ( !model_arr[ name ] ){
				model_arr[ name ] = {};
			}
			model_arr[ name ][ index ] = model;
			run_callback( event_arr[ name ], model );
		},
		/**
		 * 设置一个监听model事件
		 * @param {string} model_name 名称
		 * @param {function|string} callback 回调函数 或者 容器ID
		 * @param {string} tpl_name 模板名称
		 */
		bind: function( model_name, callback, tpl_name ){
			if ( event_arr[ model_name ] ){
				$.error( 'model event name ' + model_name + ' exist!' );
			}
			event_arr[ model_name ] = callback_check( callback, tpl_name );
		},
		/**
		 * 获取一个模型
		 */
		get: function( name, default_value ){
			if ( 'undefined' === typeof default_value ){
				default_value = null;
			}
			//别名检测
			if ( 'string' === typeof alias_arr[ name ] ){
				name = alias_arr[ name ];
			}
			//如果有.的情况
			if ( -1 !== name.indexOf( '.' ) ){
				var find_path = name.split( '.' );
				var root_data = this.get( find_path[ 0 ], {} ), i, len = find_path.length, tmp;
				for ( i = 1; i < len; ++i ){
					tmp = find_path[ i ];
					if ( 0 == tmp.length ){
						continue;
					}
					if ( !root_data || !isset( root_data[ tmp ] ) ){
						return default_value;
					}
					root_data = root_data[ tmp ];
				}
				debug( 'get', name, root_data );
				return root_data;
			}
			else {
				var result = undefined === model_arr[ name ] ? default_value : model_arr[ name ];
				debug( 'get', name, result );
				return result;
			}
		},
		/**
		 * 设置一个别名，主要用于对model的局部更新，而不触发整个model的更新
		 */
		alias: function( key, realkey, callback, tpl_name ){
			if ( 'string' !== typeof key || key.length < 1 ){
				show_error( 'alias key error' );
			}
			if ( 'string' !== typeof realkey || realkey.length < 1 ){
				show_error( 'alias realkey error' );
			}
			if ( key === realkey ){
				show_error( 'alias key can not same as realkey' );
			}
			alias_arr[ key ] = realkey;
			alias_event[ key ] = callback_check( callback, tpl_name );
		},
		/**
		 * 获取object的一个key
		 */
		get_item: function( name, item, default_value ){
			if ( 'undefined' === typeof default_value ){
				default_value = null;
			}
			if ( !model_arr[ name ] ){
				return default_value;
			}
			return model_arr[ name ][ item ] || default_value;
		},
		/**
		 * 手动触发一次事件
		 * @param name
		 */
		trigger: function( name ){
			//别名检测
			if ( isset( alias_arr[ name ] ) ){
				alisa_call( name, this.get( name ) );
				return;
			}
			if ( isset( event_arr[ name ] ) ){
				run_callback( event_arr[ name ], model_arr[ name ] );
			}
		},
		/**
		 * 是否存在某个model
		 */
		exist: function( name ){
			return isset( alias_arr[ name ] ) || isset( model_arr[ name ] );
		},
		/**
		 * 将数组model转换为object model
		 */
		arr_to_obj: function( arr_model, key ){
			var result = {}, tmp;
			for ( var i = 0; i < arr_model.length; ++i ){
				tmp = arr_model[ i ];
				if ( 'undefined' === typeof tmp[ key ] ){
					console.warn( 'key:' + key + ' not found in array model!' );
					continue;
				}
				result[ tmp[ key ] ] = tmp;
			}
			return result;
		}
	};
})();
function set_cookie( name, value, expire ){
	var strExpires = '';
	if ( expire ){
		var expires = new Date();
		expires.setTime( expires.getTime() + ( 1000 * expire ) );
		strExpires = "expires=" + expires.toGMTString() + "; ";
	}
	document.cookie = name + "=" + escape( value ) + "; " + strExpires + "path=/";
}
/**
 * 简单的抛出错误函数
 */
function show_error( msg ){
	throw Error( msg );
}
var error_manager = (function(){
	var error_handle_arr = {};

	function default_error_check( data ){
		var key = get_config( 'error_flag_key', 'error_code' );
		if ( 'undefined' !== typeof data[ key ] ){
			if ( 'string' === typeof data[ key ] && /^[\d]+]$/.test( data[ key ] ) ){
				data[ key ] = parseInt( data[ key ] );
			}
			return data[ key ];
		}
		return 0;
	}

	function default_error_handle( data ){
		seajs.use( 'window', function( newe_window ){
			newe_window.error( data.error_msg || '出错了' );
		} )
	}

	var error_check_func = default_error_check;
	return {
		/**
		 * 设置一个错误监听器
		 * @param {int|string} err_code 错误码
		 * @param {function} callback 回调函数
		 */
		add_listener: function( err_code, callback ){
			err_code += '_err';
			if ( 'function' !== typeof callback ){
				console.error( 'error listener callback is not function' );
				return;
			}
			error_handle_arr[ err_code ] = callback;
		},
		/**
		 * 批量设置错误监听器
		 * @param err_code_start
		 * @param err_code_end
		 * @param callback
		 */
		add_listener_range: function( err_code_start, err_code_end, callback ){
			for ( var i = err_code_start; i <= err_code_end; ++i ){
				this.add_listener( i, callback );
			}
		},
		/**
		 * 手动抛出一个错误
		 * @param {int|string} err_code 错误码
		 * @param {*} model 数据
		 */
		trigger: function( err_code, model ){
			err_code += '_err';
			if ( 'function' !== typeof error_handle_arr[ err_code ] ){
				default_error_handle( model );
			}
			else {
				error_handle_arr[ err_code ]( model );
			}
		},
		/**
		 * 设置判断是否发生错误的handle
		 * @param {function} handle 检测函数
		 */
		set_error_check_handle: function( handle ){
			if ( 'function' !== typeof handle ){
				console.error( 'error check funciton does not executeable!' );
				return;
			}
			error_check_func = handle;
		},
		/**
		 * 重置错误检测函数
		 */
		reset_error_check: function(){
			error_check_func = default_error_check;
		},
		/**
		 * 判断是否发生错误
		 */
		is_error: function( data ){
			return error_check_func( data );
		}
	};
})();
/**
 * 与php的 isset 函数功能类似 php可以传多个参数，该函数只传一个
 */
function isset( m ){
	return undefined !== m && null !== m;
}
/**
 * 与php的 empty 函数功能类似， 在php里，'0'是 mepty，但是在该函数里不是
 */
function empty( mix_var ){
	var undef, key, i, len;
	var empty_vars = [ undef, null, false, 0, '' ];
	for ( i = 0, len = empty_vars.length; i < len; i++ ){
		if ( mix_var === empty_vars[ i ] ){
			return true
		}
	}
	if ( 'object' === typeof mix_var ){
		for ( key in mix_var ){
			if ( mix_var.hasOwnProperty( key ) ){
				return false
			}
		}
		return true
	}
	return false
}
/**
 * 和php的intval函数功能一致
 */
function intval( mix_var, base ){
	var tmp, type = typeof mix_var;
	if ( type === 'boolean' ){
		return +mix_var;
	}
	else if ( type === 'string' ){
		tmp = parseInt( mix_var, base || 10 );
		return ( isNaN( tmp ) || !isFinite( tmp ) ) ? 0 : tmp
	}
	else if ( type === 'number' && isFinite( mix_var ) ){
		return mix_var | 0;
	}
	else {
		return 0;
	}
}
/**
 * 类似php的print_r函数
 */
function print_r( obj ){
	var print_result = [];

	function first_print( data, pre_fix ){
		if ( 'object' != typeof data ){
			print_result.push( data );
		}
		else {
			print_result.push( "Array (\n" );
			var end_str = pre_fix + ')';
			pre_fix += "    ";
			if ( null != data && data.constructor == 'Array' ){
				for ( var i = 0; i < data.length; ++i ){
					print_result.push( pre_fix + '[' + i + '] => ' );
					first_print( data[ i ], pre_fix );
					print_result.push( "\n" );
				}
			}
			else {
				for ( var p in data ){
					if ( p === '_CONSOLE_DATA_' ){
						continue;
					}
					print_result.push( pre_fix + '[' + p + '] => ' );
					first_print( data[ p ], pre_fix );
					print_result.push( "\n" );
				}
			}
			print_result.push( end_str );
		}
	}

	first_print( obj, '' );
	return print_result.join( '' );
}
/**
 * IE下没有console对象,保证代码不出错
 */
if ( !window.console ){
	window.console = {
		debug: function(){
		},
		log: function(){
		},
		info: function(){
		},
		error: function(){
		}
	};
}
if ( !window.console.groupCollapsed ){
	window.console.groupCollapsed = function(){
	};
}
if ( !window.console.groupEnd ){
	window.console.groupEnd = function(){
	};
}
if ( 'function' !== typeof console.debug ){
	console.debug = console.info ? console.info : function(){
	};
}
if ( !Date.now ){
	Date.now = function(){
		return (new Date()).valueOf();
	}
}
(function(){
	var config_arr = {};
	window.get_config = function( key, default_value ){
		if ( 'undefined' === typeof default_value ){
			default_value = null;
		}
		return 'undefined' !== typeof config_arr[ key ] ? config_arr[ key ] : default_value;
	};
	window.set_config = function( key, value ){
		config_arr[ key ] = value;
		//如果是设置服务器时间，同时要记录一下本地时间和服务器时间的差距
		if ( 'server_time' === key ){
			config_arr[ '_time_diff_' ] = value - Math.floor( Date.now() / 1000 );
		}
	};
})();
/**
 * 获取服务器时间
 */
function get_server_time(){
	return Math.floor( Date.now() / 1000 ) + get_config( '_time_diff_', 0 );
}
/**
 * 是否是测试环境
 * @return boolean
 */
function is_develop(){
	return true === get_config( 'is_debug' );
}
/**
 * 修正服务器返回结果
 * @param {object} re 原始结果
 * @param {object} fixobj 默认对象
 */
function fix_json( re, fixobj ){
	if ( 'object' !== typeof fixobj || null === fixobj ){
		fixobj = {};
	}
	if ( 'object' !== typeof re || null === fixobj ){
		return fixobj;
	}
	for ( var p in fixobj ){
		if ( !isset( re[ p ] ) ){
			re[ p ] = fixobj[ p ];
		}
	}
	return re;
}
/**
 * 在控制器打印调试数据
 */
function console_debug( data, title ){
	title = title || '服务端调试数据';
	var name, i, len;
	console.groupCollapsed( title );
	for ( name in data ){
		console.groupCollapsed( name );
		for ( i = 0, len = data[ name ].length; i < len; ++i ){
			console.info( print_r( data[ name ][ i ] ) );
		}
		console.groupEnd();
	}
	console.groupEnd();
}

seajs.config( {
	alias: {
		jquery: 'jquery/jquery/1.10.1/jquery.js',
		qrcode: 'jquery/qrcode/qrcode.js',
		select2: 'jquery/select2/select2.js',
		datepicker: 'jquery/datepicker/datepicker.js',
		core: 'jquery/core.js',
		widget: 'jquery/widget.js',
		tabs: 'jquery/tabs/tabs.js'
	}
} );