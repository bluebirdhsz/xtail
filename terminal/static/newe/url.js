define( function( require, exports ){
	var $ = require( 'jquery' );
	/**
	 * 生成参数字符串
	 */
	exports.build_query_string = function( params, url ){
		var query_str = '';
		if ( 'object' === typeof params && !empty( params ) ){
			query_str = $.param( params );
		}
		if ( 'string' !== typeof url ){
			return query_str;
		}
		if ( 0 === query_str.length ){
			return url;
		}
		return url + ( -1 === url.indexOf( '?' ) ? '?' : '&' ) + query_str;
	};
	/**
	 * 解析参数字符串
	 */
	exports.parse_query = function( query_str, name, default_value ){
		//没有传query_str或者传入空字符串，将使用当前的url的location.search字符串
		if ( 'string' !== typeof query_str || 0 === query_str.length ){
			query_str = location.search;
		}
		var first_char = query_str.charAt( 0 );
		if ( '?' === first_char || '&' === first_char ){
			query_str = query_str.substr( 1 );
		}
		var map = {};
		query_str.replace( /(^|&)([a-zA-Z_-]+)=([^&]*)/gi, function(){
			map[ RegExp.$2 ] = RegExp.$3;
			return '';
		} );
		//如果没有指定某个key，将返回整个map
		if ( 'string' !== typeof name ){
			return map;
		}
		if ( isset( map[ name ] ) ){
			return map[ name ];
		}
		if ( undefined !== default_value ){
			return default_value;
		}
		return null;
	}
} );