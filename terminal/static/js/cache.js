define( function( require ){
	'use strict';
	var number_flag = 1;
	var string_flag = 2;
	var boolean_flag = 3;
	var object_flag = 4;
	var undefined_flag = 5;
	var storage = window.localStorage;

	function supports_html5_storage(){
		try{
			return 'localStorage' in window && null != window[ 'localStorage' ];
		}
		catch ( e ){
			return false;
		}
	}

	if ( !supports_html5_storage() ){
		return null;
	}
	/**
	 * 保存数据
	 */
	function cache_save_value( key, value ){
		key = key_prefix + key;
		var tp = typeof( value );
		var save_str;
		switch ( tp ){
			case 'number':
				save_str = number_flag + ':' + value;
				break;
			case 'string':
				save_str = string_flag + ':' + value;
				break;
			case 'undefined':
				save_str = undefined_flag + ':';
				break;
			case 'boolean':
				save_str = boolean_flag + ':' + ( value ? 1 : 0 );
				break;
			case 'object':
				save_str = object_flag + ':' + JSON.stringify( value );
				break;
			default:
				throw 'unsuppot type to cache';
				break;
		}
		storage.setItem( key, save_str );
	}

	/**
	 * 保存数据
	 */
	function cache_save_get( key ){
		key = key_prefix + key;
		var tmp_value = storage.getItem( key );
		if ( 'string' !== typeof tmp_value || tmp_value.length < 2 ){
			return tmp_value;
		}
		if ( 1 !== tmp_value.indexOf( ':' ) ){
			return tmp_value;
		}
		var tp = parseInt( tmp_value.substr( 0, 1 ) );
		var content = tmp_value.substr( 2 );
		switch ( tp ){
			case number_flag:
				if ( -1 === content.indexOf( '.' ) ){
					return parseInt( content );
				}
				else {
					return parseFloat( content );
				}
				break;
			case string_flag:
				return content;
				break;
			case boolean_flag:
				return ( 1 === parseInt( content ) );
				break;
			case undefined_flag:
				return undefined;
				break;
			case object_flag:
				return JSON.parse( content );
				break;
		}
		return tmp_value;
	}

	var key_prefix = 'main_';
	return {
		get: function( key ){
			return cache_save_get( key );
		},
		set: function( key, value ){
			cache_save_value( key, value );
		},
		remove: function( key ){
			key = key_prefix + key;
			storage.removeItem( key );
		}
	};
} );