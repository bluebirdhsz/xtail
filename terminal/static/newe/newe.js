define( function( require ){
	'use strict';
	require( 'static/base.css' );
	var touch_x = 0, touch_y = 0;
	var mobile_device = ( /android|webos|iphone|ipad|ipod|blackberry|iemobile|opera mini/i.test( navigator.userAgent.toLowerCase() ) );
	var $ = require( 'jquery' );
	var newe = {
		isset: isset,
		empty: empty,
		intval: intval,
		/**
		 * 是否是开发模式
		 */
		is_dev: function(){
			//@todo
			return true;
		},
		/**
		 * 空函数, 很多地方需要写空函数,用newe.func代替
		 */
		func: function(){
		},
		/**
		 * 是否是数组
		 * @param obj
		 * @returns {boolean}
		 */
		is_array: function( obj ){
			return this.isset( obj ) && typeof obj === "object" && obj.constructor === Array;
		},
		/**
		 * 是否在数组里
		 * @param item
		 * @param arr
		 * @returns {number}
		 */
		in_array: function( item, arr ){
			if ( !this.is_array( arr ) ){
				return -1;
			}
			for ( var i = arr.length - 1; i >= 0; --i ){
				if ( item === arr[ i ] ){
					return i;
				}
			}
			return -1;
		},
		/**
		 * 截取字符器
		 */
		substr: function( str, num, join_str ){
			//默认的省略符号
			join_str = join_str || '...';
			if ( this.strlen( str ) <= num ){
				return str;
			}
			var l = str.length, tl = this.strlen( join_str );
			while ( this.strlen( str ) > num - tl ){
				str = str.substring( 0, l - 1 );
				l = l - 1;
			}
			return str + join_str;
		},
		/**
		 * 字符串长度，中文字符算2个长度
		 */
		strlen: function( str ){
			str += '';
			return str.replace( /[^\x00-\xFF]/g, '**' ).length;
		},
		/**
		 * 绑定执行过程中的this
		 */
		bind: function( fn, obj ){
			return function(){
				return fn.apply( obj, arguments );
			};
		},
		/**
		 * 移除变量
		 */
		unset: function( obj ){
			if ( 'object' === typeof obj ){
				var p;
				try{
					for ( p in obj ){
						obj[ p ] = null;
						delete( obj[ p ] );
					}
				}
				catch ( excp ){
					for ( p in obj ){
						obj[ p ] = null;
					}
				}
			}
			obj = null;
		},
		/**
		 * 页面上是否有某个id
		 */
		has_id: function( html_id ){
			return null !== document.getElementById( html_id )
		},
		/**
		 * 获取id
		 */
		get_id: function( html_id ){
			return document.getElementById( html_id );
		},
		/**
		 * 简单的继承
		 */
		extend: function( source, extend ){
			for ( var p in extend ){
				if ( this.isset( source[ p ] ) ){
					continue;
				}
				source[ p ] = extend[ p ];
			}
		},
		/**
		 * 过滤字符串的双引号过单引号，单引号为
		 */
		filt_quote: function( str ){
			if ( newe.empty( str ) ){
				return '';
			}
			str += '';
			str = str.replace( /'/g, "\\'" );
			str = str.replace( /"/g, "'" );
			return str;
		},
		is_mobile: function(){
			return mobile_device;
		},
		/**
		 * 是否ios
		 */
		is_ios: function(){
			return this.is_mobile() && !!navigator.userAgent.match( /\(i[^;]+;( U;)? CPU.+Mac OS X/ );
		},
		/**
		 * 是否android
		 */
		is_android: function(){
			return navigator.userAgent.indexOf( 'Android' ) > -1;
		},
		fix_eve: function( eve_name ){
			if ( !this.is_mobile() ){
				return eve_name;
			}
			var fix_arr = {
				click: 'touchstart',
				mousedown: 'touchstart',
				mousemove: 'touchmove',
				mouseup: 'touchend'
			};
			return fix_arr[ eve_name ] || eve_name;
		},
		/**
		 * 获取数据位置
		 */
		get_mouse: function( eve, obj ){
			if ( !obj ){
				obj = {};
			}
			var e = eve.originalEvent || eve;
			if ( e.touches ){
				if ( e.touches.length > 0 ){
					touch_x = e.touches[ 0 ].pageX;
					touch_y = e.touches[ 0 ].pageY;
				}
				obj.x = touch_x;
				obj.y = touch_y;
			}
			else {
				obj.x = eve.pageX;
				obj.y = eve.pageY;
			}
			return obj;
		},
		get_mouse_x: function( eve ){
			var e = this.get_mouse( eve );
			return e.x;
		},
		get_mouse_y: function( eve ){
			var e = this.get_mouse( eve );
			return e.y;
		},
		contains: function( a, b ){
			if ( !a || !b ){
				return false;
			}
			if ( a.contains ){
				return a != b && a.contains( b );
			}
			else {
				return a.compareDocumentPosition( b ) & 16;
			}
		},
		toggle: function( pnode ){
			pnode = pnode || this.body;
			pnode = this.jq_dom( pnode );
			pnode.find( '.newe_toggle_element' ).each( function(){
				var $this = $( this );
				if ( $this.hasClass( 'hide' ) ){
					$this.removeClass( 'hide' );
				}
				else {
					$this.addClass( 'hide' );
				}
			} );
		},
		/**
		 * 搜索事件源（未完成）
		 */
		searchEvent: function(){
			if ( window.event ){
				return window.event;
			}
			//TODO
			return null;
		},
		/**
		 * 将传入的dom转换为jquery的dom对象
		 */
		jq_dom: function( dom ){
			if ( 'string' === typeof dom ){
				dom = $( '#' + dom );
			}
			else if ( !( dom instanceof $ ) ){
				dom = $( dom );
			}
			return dom;
		},
		body: $( document.body )
	};
	return newe;
} );