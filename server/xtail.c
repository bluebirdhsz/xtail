//
// Created by bluebird on 2016-9-14.
//
#include "xtail.h"
#include "xtail_server.h"
#include "xtail_terminal.h"

//进程ID文件路径
static const char *xtail_pid_file;

//子进程个数（暂时不支持多进程，但底层框架已经支持）
int xtail_process_num = 1;

//日志
yile_log_t console_log;

//显示帮助信息
static void show_help(){
	const char *help_string = "-----------------xtail server------------------\n"
			"\n"
			"-d   damond\n"
			"-c   config file\n"
			"Sample：./xtail -c config.ini\n"
			"------------------------------------------------------------\n";
	printf( "%s", help_string );
}

int main( int argc, char *argv[] ){
	const char *config_file = NULL;
	int arg_c;
	int daemon_flag = 0;
	while ( -1 != ( arg_c = getopt( argc, argv, "c:d" ))){
		switch ( arg_c ){
			case 'c':
				config_file = strdup( optarg );
				break;
			case 'd':
				daemon_flag = 1;
				break;
			default:
				show_help();
				return YILE_ERROR;
		}

	}
	//没有指定config_file
	if ( NULL == config_file ){
		show_help();
		return YILE_ERROR;
	}
	const char *section = "XTAIL";
	yile_ini_t ini_obj = { 0 };
	if ( YILE_OK != yile_ini_parse( config_file, &ini_obj )){
		return YILE_ERROR;
	}
	//如果是守护进程, 要处理输出日志
	if ( daemon_flag ){
		const char *log_path = yile_ini_get_string( section, "log_path", "/data/logs/", &ini_obj );
		yile_process_daemon();
		yile_log_init( &console_log, log_path, section, LOG_SPLIT_BY_DAY );
	}

	//生成进程pid文件
	const char *tmp_pid_file = yile_ini_get_string( section, "pid_file", "/tmp/xtail.pid", &ini_obj );
	if ( YILE_OK != yile_process_pid_access( tmp_pid_file )){
		return YILE_ERROR;
	}
	xtail_pid_file = strdup( tmp_pid_file );
	int pid = getpid();
	if ( YILE_OK != yile_process_pid_touch( xtail_pid_file, pid )){
		fprintf(stderr, "main() Write pid to file error\n" );
		return -1;
	}
	//epoll初始化
	if ( YILE_OK != xtail_main_event_set()){
		return YILE_ERROR;
	}
	//内网监听事件初始化
	const char *lan_bind_ip = yile_ini_get_string( section, "server_host", "127.0.0.1", &ini_obj );
	int lan_ini_port = yile_ini_get_int( section, "server_port", -1, &ini_obj );
	if ( -1 == lan_ini_port ){
		fprintf(stderr, "Server port read failed!\n" );
		return YILE_ERROR;
	}
	//打开监听
	if ( YILE_OK != xtail_server_listen( lan_bind_ip, lan_ini_port )){
		return YILE_ERROR;
	}
	//terminal网络事件初始化
	const char *terminal_bind_ip = yile_ini_get_string( section, "terminal_host", "0.0.0.0", &ini_obj );
	int terminal_ini_port = yile_ini_get_int( section, "terminal_port", -1, &ini_obj );
	if ( -1 == terminal_ini_port ){
		fprintf(stderr, "Terminal port read failed!\n" );
		return YILE_ERROR;
	}
	//打开监听
	if ( YILE_OK != xtail_terminal_listen( terminal_bind_ip, terminal_ini_port )){
		return YILE_ERROR;
	}
	//初始化进程（暂时不支持多进程）
	yile_process_conf_t process_conf = {0};
	process_conf.child_num = xtail_process_num;
	yile_process_init( &process_conf );

	//清理ini
	yile_ini_clear( &ini_obj );
	//保存avc argv
	yile_process_save_arg( argc, argv );

	//进程开始工作
	yile_process_start_work();
	//正常退出，删除pid文件
	yile_process_pid_unlink( xtail_pid_file );
	return 0;
}