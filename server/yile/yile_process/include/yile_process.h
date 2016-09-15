#ifndef YILE_PROCESS_HEAD
#define YILE_PROCESS_HEAD
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <libgen.h>
#include <sys/time.h>
#include "yile_net.h"

#define YILE_INVALID_PID					-1

//新建立进程的方式+
#define YILE_PROCESS_RESPAWN				-1

//通知子进程优雅退出
#define YILE_PROCESS_COMMAND_QUIT			1
//通知子进程强制退出
#define YILE_PROCESS_COMMAND_KILL			2
//心跳
#define YILE_PROCESS_COMMAND_HEARTBEAT		3
//交出listenfd
#define	YILE_PROCESS_COMMAND_RELEASE_ACCEPT	4
//接受请求
#define YILE_PROCESS_COMMAND_ACCEPT			5
//接收到其它子进程的fd
#define	YILE_PROCESS_COMMAND_BROTHER		6
//通知路由器模块加入(待优化)
#define YILE_PROCESS_COMMAND_MODULE_JOIN	10007
//通知路由器内网加入(待优化)
#define YILE_PROCESS_COMMAND_LAN_JOIN		10008
//唤醒进程读取共享内存的数据
#define YILE_PROCESS_COMMAND_WAKEUP			10009


//进程间通信的协议个数
#define YILE_PROCESS_PROTOCOL_NUM			10

//进程间通信是否有文件描述符
#define yile_process_ipc_need_fd( command ) ( command == YILE_PROCESS_COMMAND_BROTHER || command == YILE_PROCESS_COMMAND_MODULE_JOIN || command == YILE_PROCESS_COMMAND_LAN_JOIN )

//子进程运行标志
#define YILE_PROCESS_RUN_FLAG 0 == yile_process_quit

//是否多进程
#define YILE_PROCESS_MULTI_CHILD() yile_process_num > 1
//进程控制
typedef struct yile_process_conf_s yile_process_conf_t;

//进程结构体
typedef struct yile_process_s yile_process_t;

//进程间通信的结构体
typedef struct yile_process_ipc_s yile_process_ipc_t;

//在fork前的回调函数
typedef int( *yile_process_fork_pt )( int process_slot_id );

//子进程的初始化函数
typedef void (*yile_process_init_pt)();

//子进程的loop函数
typedef void (*yile_process_loop_pt)();

//进程间ipc
typedef int (*yile_process_ipc_call_pt)( yile_process_ipc_t *msg );

//接收请求的
typedef struct yile_process_listen_s yile_process_listen_t;

//进程优雅退出标志
extern sig_atomic_t yile_process_quit;

#if defined __cplusplus
extern "C" {
#endif

//进程的配置
struct yile_process_conf_s {
	int						heartbeat;		//心跳间隔
	int						child_num;		//子进程数量
	int						accept_round_size; //每进程依次接收多少个请求
	const char				*group_name;	//子进程运行的group_id
	const char				*user_name;		//子进程运行的user_id
	yile_process_init_pt	child_init;		//子进程初始化函数
	yile_process_loop_pt	child_loop;		//子进程循环函数
	yile_process_fork_pt	child_fork;		//子进程fork前回调
	yile_process_ipc_call_pt ipc_callback;	//进程间ipc回调
};

//进程结构体
struct yile_process_s {
	int						pid;			//进程id
	int						pairfd[ 2 ];	//用于进程通信的
	uint32_t				ping;			//最后一次心跳
	unsigned				exited:1;		//是否已经退出
	unsigned				exiting:1;		//是否正在退出
	yile_connection_t		*channel;		//进程通信通道
};

//进程间通信的结构体
struct yile_process_ipc_s {
	int						command;		//命令
	int						fd;				//传递的fd
	int						pid;			//进程的id
	//module_id和channel_id是为了路由间传递fd
	uint8_t					module_id;		//模块id
	uint8_t					channel_id;		//通道id
	uint16_t				process_slot;	//子进程的编号
	//listen_slot为了通知子进程accept指定的listen_slot
	uint16_t				listen_slot;	//监听的slot
};

//接收请求的
struct yile_process_listen_s {
	uint32_t				taken_slot;		//占用该listen_fd的进程编号
	int						is_taken;		//是否被占用
	yile_connection_t		*listen_fd;		//监听进程
};

//socketpair 0:主进程收发数据  1:子进程收发数据

/**
 * 主进程进入等待事件状态
 */
void yile_process_main_loop();

/**
 * 设置子进程的loop
 */
void yile_process_set_loop( yile_process_loop_pt loop_func );

/**
 * 初始化进程
 */
int yile_process_init( yile_process_conf_t *process_conf );

/**
 * 开始进程活动
 */
void yile_process_start_work();

/**
 * 进程间通信_发数据
 */
int yile_process_ipc_send( int fd, yile_process_ipc_t *send_data );

/**
 * 进程间通信
 */
int yile_process_ipc( int process_id, yile_process_ipc_t *ipc_data );

/**
 * 进程间通信_读数据
 */
int yile_process_ipc_read( yile_connection_t *tmp );

/**
 * 主进程将新生成的进程信息发送给其它子进程
 */
int yile_process_child_sync( yile_process_t *tmp_process, int child_slot );

/**
 * 增加通过sokcet监听
 */
yile_connection_t *yile_process_socket_listen( const char *host, int port );

/**
 * 进程文件是否存在
 */
int yile_process_pid_access( const char *pid_file );

/**
 * 生成pid文件
 */
int yile_process_pid_touch( const char *pid_file, int pid );

/**
 * 删除进程文件
 */
int yile_process_pid_unlink( const char *pid_file );

/**
 * 返回子进程的slit_id
 */
int yile_process_child_id();

/**
 * 后台守护进程
 */
int yile_process_daemon();

/**
 * 保存参数
 */
void yile_process_save_arg( int argc, char **argv );

/**
 * 设置标题
 */
void yile_process_set_title( const char *title_str );

/**
 * 获取子进程个数
 */
int yile_process_child_num();

/**
 * 获取程序运行的目录
 */
int yile_process_getpwd( char *pwd, ssize_t pwd_len );
/**
 * 获取进程详细数据
 */
yile_process_t *yile_process_get( int process_id );

#if defined __cplusplus
}
#endif
#endif