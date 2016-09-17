<?php
require_once 'proto.php';
/**
 * Class XTail php 客户端
 */
class XTail{
	/** 调试信息 */
	const MSG_DEBUG = 1;
	/** 警告信息 */
	const MSG_WARN = 2;
	/** 错误信息 */
	const MSG_ERROR = 4;
	/** 协议头长度（固定6位） */
	const PROTOCOL_HEAD_LEN = 6;
	/** 加入服务器成功返回的协议ID */
	const JOIN_RE_PROTOCOL_ID = 102;
	/** @var int tail 消息类型（默认全部打开 */
	private static $tail_level = 7;
	/** @var resource 与服务器连接的资源id（为0表示没有创建连接） */
	private static $tail_fd = 0;
	/** @var array 默认配置 */
	private static $config = array(
		'host' => '127.0.0.1',
		'port' => 7777,
		'secret' => 'www.ffan.com',
		//连接超时时间
		'timeout' => 2
	);

	/**
	 * tail debug消息
	 * @param mixed $msg_var 消息变量（可以是任意类型） 
	 * @param string $group 分组名，必须！！是IP地址格式（默认是以当前ip地址为分组） 
	 */
	public static function debug( $msg_var, $group = null ){
		if ( !( self::$tail_level & self::MSG_DEBUG ) ){
			return;
		}
		self::push( $msg_var, self::MSG_DEBUG, $group );
	}
	
	/**
	 * tail debug消息
	 * @param mixed $msg_var 消息变量（可以是任意类型）
	 * @param string $group 分组名，必须！！是IP地址格式（默认是以当前ip地址为分组）
	 */
	public static function warn( $msg_var, $group = null ){
		if ( !( self::$tail_level & self::MSG_WARN ) ){
			return;
		}
		self::push( $msg_var, self::MSG_WARN, $group );
	}
	
	/**
	 * tail debug消息
	 * @param mixed $msg_var 消息变量（可以是任意类型）
	 * @param string $group 分组名，必须！！是IP地址格式（默认是以当前ip地址为分组）
	 */
	public static function error( $msg_var, $group = null ){
		if ( !( self::$tail_level & self::MSG_ERROR ) ){
			return;
		}
		self::push( $msg_var, self::MSG_ERROR, $group );
	}

	/**
	 * 推送消息
	 * @param mixed $msg_var
	 * @param int $msg_type
	 * @param string $group
	 */
	private static function push( $msg_var, $msg_type, $group ){
		if ( 0 === self::$tail_fd && !self::join_server() ){
			return;
		}
		if ( null === $group ){
			$group = self::get_ip();
		}
		$group_id = ip2long( $group );
		if ( false === $group ){
			trigger_error( 'Group is not ip address' );
			return;
		}
		//hostname 用于在客户端判断是哪一个网站推送的消息
		if ( isset( self::$config[ 'host_name' ] ) ){
			$host_name = self::$config[ 'host_name' ];
		}
		//cli模式
		elseif ( 'cli' === PHP_SAPI ){
			global $argv;
			$host_name = !empty( $argv[ 0 ] ) ? join( ' ', $argv ) : 'php cli';
		}
		else{
			$host_name = isset( $_SERVER[ 'HTTP_HOST' ] ) ? $_SERVER[ 'HTTP_HOST' ] : 'localhost';
		}
		$push_arr = array(
			'group_id' => $group_id,
			'host_name' => $host_name,
			'msg_type' => $msg_type,
			'msg' => self::var_format( $msg_var )
		);
		$result = php_pack_push_msg( $push_arr );
		$re = fwrite( self::$tail_fd, $result );
		if ( $re !== strlen( $result ) ){
			self::close();
		}
	}

	/**
	 * 加入服务器
	 * @return bool
	 */
	private static function join_server(){
		$host = 'tcp://'. self::$config[ 'host' ] .':'. self::$config[ 'port' ];
		$fd = stream_socket_client( $host, $err_code, $err_msg, self::$config[ 'timeout' ] );
		if ( !$fd ){
			trigger_error( 'Can not join tail server '. $host .' err_code:'. $err_code. ' reason: '. $err_msg, E_CORE_WARNING );
			return false;
		}
		//设置成阻塞模式
		stream_set_blocking( $fd, 1 );
		//设置读取数据的timeout
		stream_set_timeout( $fd, self::$config[ 'timeout' ] );
		$proto_data = array(
			'auth_str' => self::$config[ 'secret' ]
		);
		$send_data = php_pack_join_xtail( $proto_data );
		if ( false === fwrite( $fd, $send_data ) ){
			trigger_error( 'Can not send tail msg' );
			return false;
		}
		$read_re = fread( $fd, 100 );
		if ( empty( $read_re ) ){
			trigger_error( 'Tail server has no response!' );
			return false;
		}
		$head = unpack( "Llen/Sproto_id", substr( $read_re, 0, self::PROTOCOL_HEAD_LEN ) );
		$body_data = substr( $read_re, self::PROTOCOL_HEAD_LEN );
		//数据出错
		if ( self::JOIN_RE_PROTOCOL_ID !== $head[ 'proto_id' ] || $head[ 'len' ] !== strlen( $body_data ) ){
			trigger_error( 'Tail server protocol error!' );
			return false;
		}
		$join_re = php_unpack_join_result( $body_data );
		if ( !isset( $join_re[ 'status' ] ) || 0 !== $join_re[ 'status' ] ){
			$msg = isset( $join_re[ 'message' ] ) ? $join_re[ 'message' ] : 'Unkown reason';
			trigger_error( 'Can not join tail server! reason:'. $msg );
			return false;
		}
		self::$tail_fd = $fd;
		return true;
	}

	/**
	 * 断开连接
	 */
	private static function close(){
		fclose( self::$tail_fd );
		self::$tail_fd = 0;
	}

	/**
	 * 变量格式化
	 * @param mixed $var 变量
	 * @return string
	 */
	private static function var_format( $var ){
		$type = gettype( $var );
		switch ( $type ){
			case 'boolean':
				$result_str = $var ? 'True' : 'False';
			break;
			case 'integer':
			case 'double':
				$result_str = (string)$var;
			break;
			case 'NULL':
				$result_str = 'NULL';
			break;
			case 'resource':
				$result_str = 'Resource:' . get_resource_type( $var );
			break;
			case 'string':
				$result_str = $var;
			break;
			case 'object':
				$result_str = 'Class of "' . get_class( $var ) . '"';
				$result_str .= print_r( $var, true );
			break;
			case 'array':
			default:
				$result_str = print_r( $var, true );
			break;
		}
		return $result_str;
	}

	/**
	 * 获取IP地址
	 * @return string
	 */
	private static function get_ip(){
		static $client_ip;
		if ( !$client_ip ){
			$except = 'unknown';
			if ( isset( $_SERVER[ 'HTTP_CLIENT_IP' ] ) && 0 != strcasecmp( $_SERVER[ 'HTTP_CLIENT_IP' ], $except ) ){
				$client_ip = $_SERVER[ 'HTTP_CLIENT_IP' ];
			}
			elseif ( isset( $_SERVER[ 'HTTP_X_FORWARDED_FOR' ] ) && 0 != strcasecmp( $_SERVER[ 'HTTP_X_FORWARDED_FOR' ], $except ) ){
				$client_ip = $_SERVER[ 'HTTP_X_FORWARDED_FOR' ];
				$pos = strpos( $client_ip, ',' );
				if ( false !== $pos ){
					$client_ip = substr( $client_ip, 0, $pos );
				}
			}
			elseif ( isset( $_SERVER[ 'REMOTE_ADDR' ] ) && 0 != strcasecmp( $_SERVER[ 'REMOTE_ADDR' ], $except ) ){
				$client_ip = $_SERVER[ 'REMOTE_ADDR' ];
			}
			else{
				$client_ip = '127.0.0.1';
			}
		}
		return $client_ip;
	}
}
