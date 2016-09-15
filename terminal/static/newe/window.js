define( function( require ){
	'use strict';
	var $ = require( 'jquery' );
	var newe = require( 'newe' );
	var newe_layer = require( 'layer/layer.js' );
	var current_win_id = [];
	var current_tip_id = [];

	function push_tip_id( id ){
		current_tip_id.push( id );
		current_tip_id.push( Date.now() );
	}

	$( document ).bind( 'click', function( eve ){
		var target = eve.target || eve.srcElement;
		if ( !target ){
			return;
		}
		if ( 'close' === target.getAttribute( 'i' ) ){
			close_win_by_dom( target );
		}
		if ( 0 === current_tip_id.length ){
			return;
		}
		var new_tip_array = [], now = Date.now();
		while ( current_tip_id.length > 0 ){
			var tip_time = current_tip_id.pop();
			var tip_id = current_tip_id.pop();
			var tip_body = newe.get_id( 'layui-layer' + tip_id );
			if ( newe.contains( tip_body, target ) || now - tip_time < 150 ){
				new_tip_array.push( tip_id );
				new_tip_array.push( tip_time );
			}
			else {
				clear_callback( tip_id );
				newe_layer.close( tip_id );
			}
		}
		if ( new_tip_array.length > 0 ){
			current_tip_id = new_tip_array;
		}
	} );
	/**
	 * 清理回调
	 */
	function clear_callback( index ){
		delete( layer_tip_callback[ '1_' + index ] );
		delete( layer_tip_callback[ '2_' + index ] );
	}

	//有用
	var layer_tip_callback = {};
	window.newe_window_tip_btn = function( btn, tip_id ){
		var callback = layer_tip_callback[ btn + '_' + tip_id ];
		if ( 'function' === typeof callback ){
			callback();
		}
		newe_layer.close( tip_id );
		clear_callback( tip_id );
	};
	$( document ).keydown( function( event ){
		if ( 27 !== event.keyCode ){
			return;
		}
		while ( current_win_id.length > 0 ){
			var id = current_win_id.pop();
			//如果窗口带beforeclose 属性, 必须执行回调函数, 如果返回false, 表示不能关闭窗口
			if ( newe.is_array( id ) ){
				var func = id[ 1 ];
				if ( false === func() ){
					current_win_id.unshift( id );
					return;
				}
				id = id[ 0 ];
			}
			if ( newe.has_id( 'layui-layer' + id ) ){
				newe_layer.close( id );
				break;
			}
		}
	} );
	function layer_tip_arg( arg ){
		var align_tip = {
			top: 1, right: 2, bottom: 3, left: 4
		};
		var new_arg = {};
		if ( !arg ){
			arg = {};
		}
		var tp = typeof arg;
		if ( 'function' === tp ){
			new_arg.end = arg;
		}
		else if ( 'function' === typeof arg.callback ){
			new_arg.end = arg.callback;
		}
		//兼容老版本的参数
		if ( 'string' === tp ){
			arg = { align: arg };
		}
		if ( arg.align && isset( align_tip[ arg.align ] ) ){
			new_arg.tips = align_tip[ arg.align ];
		}
		if ( !isset( new_arg.tips ) ){
			new_arg.tips = 2;
		}
		if ( isset( arg.color ) ){
			new_arg.tips = [ new_arg.tips, arg.color ];
		}
		if ( empty( arg.time ) ){
			new_arg.time = 0;
		}
		else {
			new_arg.time |= 0;
		}
		new_arg.time *= 1000;
		return new_arg;
	}

	var win_body_css = 'newe_window_content_body', tiny_btn_div = 'newe_tiny_tip_content';
	var tiny_btn_id = 1;

	/**
	 * 生成按钮
	 */
	function win_tiny_btn( msg, btn, tip_id, ok_btn, cancel_btn ){
		ok_btn = '确定';
		tip_id |= 0;
		var result = [];
		if ( 0 === tip_id ){
			result.push( '<div class="newe_tiny_box_body" id="' + tiny_btn_div + tiny_btn_id + '">' );
		}
		result.push( msg );
		result.push( '<div class="newe_tiny_box_btn">' );
		if ( ( btn & 1 ) ){
			result.push( '<a class="newe_btn layui-layer-btn0"' );
			if ( tip_id > 0 ){
				result.push( ' onclick="newe_window_tip_btn(1,' + tip_id + ')"' );
			}
			result.push( '>' + ok_btn + '</a>' );
		}
		if ( ( btn & 2 ) ){
			cancel_btn = '取消';
			result.push( ' <a class="newe_btn layui-layer-btn1"' );
			if ( tip_id > 0 ){
				result.push( ' onclick="newe_window_tip_btn(2,' + tip_id + ')"' );
			}
			result.push( '>' + cancel_btn + '</a>' );
		}
		result.push( '</div>' );
		if ( 0 === tip_id ){
			result.push( '</div>' );
		}
		return result.join( '' );
	}

	/**
	 * 添加上按钮的onclick
	 */
	function win_tiny_btn_onclick( msg ){
		$( '#' + tiny_btn_div + tiny_btn_id ).html( msg );
		tiny_btn_id++;
	}

	var newe_win = {
		/**
		 * 弹出框
		 * @param {string} msg 弹出框显未内容
		 * @param {*} callback 点击确认按钮后的回调函数
		 * @param {int} width 宽度
		 * @param {string} title 窗口的标志
		 * @param {string} btn_text 按钮文字
		 * @param {int} icon 图标( 0 -8 )
		 */
		alert: function( msg, callback, width, title, btn_text, icon ){
			title = '系统消息';
			icon |= 0;
			if ( icon > 7 ){
				icon = 0;
			}
			var arg = { icon: icon, title: title };
			width = width || 400;
			if ( width > 0 ){
				arg.area = width + 'px';
			}
			arg.btn = '确定';
			newe_layer.alert( msg, arg, function( index ){
				if ( callback ){
					newe.run( callback );
				}
				newe_layer.close( index );
			} );
		},
		/**
		 * 弹出确认操作框
		 * @param {string} msg 弹出框显未内容
		 * @param {*} ok_callback 点击确认按钮后的回调函数
		 * @param {*} cancel_callback 点击确认按钮后的回调函数
		 * @param {int} width 宽度
		 * @param {string} title 窗口的标志
		 * @param {string} ok_btn_text 确定按钮文字
		 * @param {string} cancel_btn_text 取消按钮文字
		 */
		confirm: function( msg, ok_callback, cancel_callback, width, title, ok_btn_text, cancel_btn_text ){
			title = '系统消息';
			var arg = { icon: 3, title: title };
			width = width || 400;
			if ( width > 0 ){
				arg.area = width + 'px';
			}
			arg.btn = [ '确定', '取消' ];
			newe_layer.confirm( msg, arg, function( index ){
				if ( ok_callback ){
					newe.run( ok_callback );
				}
				newe_layer.close( index );
			}, function( index ){
				if ( cancel_callback ){
					newe.run( cancel_callback );
				}
				newe_layer.close( index );
			} );
		},
		/**
		 * 弹出错误消息
		 * @param {string} msg 弹出框显未内容
		 * @param {*} callback 点击确认按钮后的回调函数
		 * @param {int} width 宽度
		 * @param {string} title 窗口的标志
		 * @param {string} btn_text 按钮文字
		 */
		error: function( msg, callback, width, title, btn_text ){
			title = '系统错误';
			this.alert( msg, callback, width, title, btn_text, 2 );
		},
		/**
		 * 弹出成功消息
		 * @param {string} msg 弹出框显未内容
		 * @param {*} callback 点击确认按钮后的回调函数
		 * @param {int} width 宽度
		 * @param {string} title 窗口的标志
		 * @param {string} btn_text 按钮文字
		 */
		ok: function( msg, callback, width, title, btn_text ){
			title = title || '成功';
			this.alert( msg, callback, width, title, btn_text, 1 );
		},
		/**
		 * 气泡alert框
		 * @param {*} p_node 依附对象的DOM
		 * @param {string} msg 消息内容
		 * @param {*} ok_callback 点确定的回调
		 * @param {*} arg 方向 或者 各种参数
		 */
		tiny_alert: function( p_node, msg, ok_callback, arg ){
			arg = layer_tip_arg( arg );
			arg.time = 0;
			var ok_btn_text = arg.ok_btn;
			var tmp_msg = win_tiny_btn( msg, 1, 0, ok_btn_text );
			if ( 'string' === typeof p_node ){
				p_node = '#' + p_node;
			}
			var tip_id = newe_layer.tips( tmp_msg, p_node, arg );
			push_tip_id( tip_id );
			if ( ok_callback ){
				layer_tip_callback[ '1_' + tip_id ] = ok_callback;
			}
			arg.tipsMore = true;
			var new_msg = win_tiny_btn( msg, 1, tip_id, ok_btn_text );
			win_tiny_btn_onclick( new_msg );
		},
		/**
		 * 气泡提示框
		 * @param {*} p_node 依附对象的DOM
		 * @param {string} msg 消息内容
		 * @param {*} arg 方位 left, right top, bottom
		 */
		tiny_box: function( p_node, msg, arg ){
			arg = layer_tip_arg( arg );
			if ( 'string' === typeof p_node ){
				p_node = '#' + p_node;
			}
			if ( isset( arg.time ) ){
				arg.time *= 1000;
			}
			arg.tipsMore = true;
			push_tip_id( newe_layer.tips( msg, p_node, arg ) );
		},
		/**
		 * 气泡提示框
		 * @param {*} p_node 依附对象的DOM
		 * @param {*} msg 消息内容
		 * @param {*} arg 方位 left, right top, bottom
		 */
		tiny_error: function( p_node, msg, arg ){
			arg = layer_tip_arg( arg );
			if ( 'string' === typeof p_node ){
				p_node = '#' + p_node;
			}
			var tmp = arg.tips;
			if ( 'number' !== typeof tmp ){
				tmp = arg.tips[ 0 ];
			}
			arg.tipsMore = true;
			arg.tips = [ tmp, 'red' ];
			push_tip_id( newe_layer.tips( msg, p_node, arg ) );
		},
		/**
		 * 气泡confirm框
		 * @param {*} p_node 依附对象的DOM
		 * @param {string} msg 消息内容
		 * @param {*} ok_callback
		 * @param {*} arg 参数 或者 方位
		 * @param {*} cancel_callback 点取消的回调
		 */
		tiny_confirm: function( p_node, msg, ok_callback, arg, cancel_callback ){
			arg = layer_tip_arg( arg );
			arg.time = 0;
			var tmp_msg = win_tiny_btn( msg, 3, 0, arg.ok_btn, arg.cancel_btn );
			if ( 'string' === typeof p_node ){
				p_node = '#' + p_node;
			}
			var tip_id = newe_layer.tips( tmp_msg, p_node, arg );
			push_tip_id( tip_id );
			if ( ok_callback ){
				layer_tip_callback[ '1_' + tip_id ] = ok_callback;
			}
			if ( cancel_callback ){
				layer_tip_callback[ '2_' + tip_id ] = cancel_callback;
			}
			arg.tipsMore = true;
			var new_msg = win_tiny_btn( msg, 3, tip_id, arg.ok_btn, arg.cancel_btn );
			win_tiny_btn_onclick( new_msg );
		},
		modal: alert, //和alert方法一致
		/**
		 * 在页面中间出现提示, 一定时间后自动关闭
		 * @param {string} msg 消息内容
		 * @param {*} callback 回调
		 * @param {int} last_time 持续时间
		 * @param {string} parent 附着在哪个上面
		 * @param {string} align 方位
		 */
		box: function( msg, callback, last_time, parent, align ){
			var arg = { align: align };
			arg = layer_tip_arg( arg );
			if ( -1 !== last_time ){
				last_time = intval( last_time );
				if ( 0 === last_time ){
					last_time = 3;
				}
				arg.time = last_time * 1000;
			}
			if ( 'string' === typeof parent ){
				parent = '#' + parent;
			}
			if ( callback ){
				arg.end = function(){
					newe.run( callback );
				}
			}
			arg.tipsMore = true;
			push_tip_id( newe_layer.tips( msg, parent, arg ) );
		},
		/**
		 * 简单的消息提示框
		 * @param {string} msg 提示内容
		 * @param {*} options 参数 数字: 图标编号 对象：参数
		 * @param {*} callback 回调函数
		 */
		msg: function( msg, options, callback ){
			if ( 'string' === typeof options ){
				options |= 0;
			}
			if ( 'number' === typeof options && options < 7 ){
				options = { icon: options };
			}
			if ( 'function' !== typeof callback ){
				callback = function(){}
			}
			options = options || {};
			if ( isset( options.modal ) ){
				options.shade = [ 0.2, 'black' ];
				options.shadeClose = true;
			}
			current_win_id.push( newe_layer.msg( msg, options, callback ) );
		},
		/**
		 * 弹出一个框,如果这个框存在了,将不会再打开, 而只是刷新内容
		 */
		open: function( content, option ){
			if ( 'object' !== typeof option ){
				option = {};
			}
			var attr = [ 'class="' + win_body_css + '"' ];
			if ( option.id ){
				var win = newe.get_id( option.id );
				if ( win ){
					if ( win_body_css === win.className ){
						var content_div = $( '#' + option.id );
						content_div.find( '*' ).off();
						content_div.html( content );
					}
					else if ( newe.is_dev() ){
						this.alert( 'ID = ' + option.id + ' 的元素已经存在，请更换窗口ID' );
					}
					return;
				}
				attr.push( 'id="' + option.id + '"' );
			}
			var arg2 = { type: 1 };
			//遮罩
			if ( true === option.modal ){
				arg2.shade = [ 0.2, '#eeeeee' ];
			}
			else {
				arg2.shade = false;
			}
			if ( option.title ){
				arg2.title = option.title;
				arg2.closeBtn = 1;
			}
			else {
				arg2.title = false;
				arg2.closeBtn = false;
			}
			//空白
			if ( !isset( option.padding ) ){
				option.padding = 0;
			}
			else {
				option.padding = intval( option.padding );
			}
			attr.push( 'style="padding:' + option.padding + 'px"' );
			//高宽
			if ( option.width ){
				if ( option.height ){
					arg2.area = [ arg_width( option.width ), arg_width( option.height ) ];
				}
				else {
					arg2.area = arg_width( option.width );
				}
			}
			//没有设置宽度, 那就自动判断
			else {
				var tmp_div = $( '<span style="display:inline-block;visibility: hidden">' + content + '</span>' ).appendTo( $( document.body ) );
				var tmp_w = tmp_div.width();
				if ( tmp_w < 200 ){
					tmp_w = 200;
				}
				tmp_div.remove();
				tmp_div = null;
				arg2.area = tmp_w + 'px';
			}
			if ( 'function' === typeof option.onclose ){
				arg2.end = option.onclose;
			}
			if ( 'function' === typeof option.beforeclose ){
				arg2.cancel = option.beforeclose;
			}
			if ( option.zIndex ){
				arg2.zIndex = option.zIndex;
			}
			arg2.content = '<div ' + attr.join( ' ' ) + '>' + content + '</div>';
			var wid = newe_layer.open( arg2 );
			if ( arg2.cancel ){
				current_win_id.push( [ wid, option.beforeclose ] );
			}
			else {
				current_win_id.push( wid );
			}
		},
		/**
		 * 获取一个元素
		 */
		capture: function( dom ){
			var node = newe.jq_dom( dom );
			var arg = {
				type: 1,
				shade: false,
				title: false,
				content: node, cancel: function( index ){
					newe_layer.close( index );
					this.content.show();
				}
			};
			current_win_id.push( newe_layer.open( arg ) );
		},
		/**
		 * 关闭一个窗口
		 */
		close: function( id ){
			if ( !newe.has_id( id ) ){
				return;
			}
			close_win_by_dom( newe.get_id( id ) );
		}
	};

	/**
	 * 清理宽度
	 */
	function arg_width( width ){
		if ( /^[\d]+$/.test( width ) ){
			return width + 'px';
		}
		return width;
	}

	/**
	 * 关闭一个窗口
	 */
	function close_win_by_dom( dom ){
		dom = newe.jq_dom( dom );
		while ( dom && dom.length > 0 ){
			if ( dom.hasClass( 'layui-layer' ) ){
				var tmp_id = dom.attr( 'id' );
				if ( tmp_id ){
					newe_layer.close( tmp_id.replace( /[^\d]/g, '' ) );
				}
				break;
			}
			dom = dom.parent();
		}
	}

	return newe_win;
} );