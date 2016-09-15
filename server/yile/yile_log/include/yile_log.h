#ifndef YILE_PHP_LOG_HEAD
#define YILE_PHP_LOG_HEAD
#include "yile.h"
#include <stdio.h>
#include <time.h>
#include "string.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOG_SPLIT_BY_DAY	86400
#define LOG_SPLIT_BY_HOUR	3600

typedef struct yile_log_s yile_log_t;
struct yile_log_s{
	int				fd;					//打开的文件
	int				timestr;			//时间
	int				split_type;			//日志文件间隔类型 1:按天划分 2:按小时划分
	char			*log_path;			//日志路径
	char			*log_name;			//日志名
};

#if defined __cplusplus
extern "C" {
#endif

/**
 * 初始化yile_log
 */
int yile_log_init( yile_log_t *tmp_log, const char *log_path, const char*log_name, int split_type );

/**
 * 生成新的yile_log
 */
yile_log_t *yile_log_new( const char *log_path, const char *log_name, int split_type );

/**
 * 日志滚动
 */
int yile_log_roll( yile_log_t *tmp );
#if defined __cplusplus
}
#endif
#endif
