#ifndef YILE_INI_HEAD
#define YILE_INI_HEAD

#if defined __cplusplus
extern "C" {
#endif
//section
//ini解析类
typedef struct yile_ini_t yile_ini_t;
typedef struct yile_ini_section_t yile_ini_section_t;
//配置项
typedef struct yile_ini_item_t yile_ini_item_t;
struct yile_ini_t{
	yile_ini_section_t	*head_section;
};

struct yile_ini_section_t{
	int					section_len;		//section字符串长度
	char				*section_name;		//section名称 如果为NULL表示全局配置
	yile_ini_item_t		*item_head;			//第一个选项
	yile_ini_section_t	*next;
};
struct yile_ini_item_t{
	int					item_len;			//配置字符串长度
	char				*item_name;			//配置名称
	char				*item_value;		//配置值
	yile_ini_item_t		*next;
};
//读一个section
typedef void(*yile_ini_list_pt)( const char *section_name, yile_ini_t *ini_obj );

/**
 * 解析ini文件
 */
int yile_ini_parse( const char *filename, yile_ini_t *yileini );

/**
 * 读取整数
 */
int yile_ini_get_int( const char *section, const char *key, int default_value, yile_ini_t *yileini );

/**
 * 读取布尔值
 */
int yile_ini_get_bool( const char *section, const char *key, int default_value, yile_ini_t *yileini );

/**
 * 读取字符串
 */
const char *yile_ini_get_string( const char *section, const char *key, const char *default_value, yile_ini_t *yileini );

/**
 * 是否有某个section
 */
int yile_ini_has_section( const char *section, yile_ini_t *yileini );

/**
 */
void yile_ini_clear( yile_ini_t *yileini );

/**
 * 逐一解析每一个section
 */
int yile_ini_list_section( yile_ini_t *ini_obj, const char *section_list_str, yile_ini_list_pt callback );

#if defined __cplusplus
}
#endif
#endif