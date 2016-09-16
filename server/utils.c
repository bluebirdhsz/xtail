//
// Created by bluebird on 2016-9-16.
//

#include "utils.h"
#include "string.h"
/**
 * 从字符串中找到某一段
 * 返回NULL表示没有找到或者字符串太长了，其它情况返回字符串指针
 */
char *str_search( char *str, char *beg_str, char *end_str, char *result, int result_len ){
	if ( NULL == beg_str || NULL == str ){
		return NULL;
	}
	//找到begpos出现的位置
	char *tmp_result = strstr( str, beg_str );
	//没有出现
	if ( NULL == tmp_result ){
		return NULL;
	}
	//不要beg_str字符串本身
	tmp_result += strlen( beg_str );
	char *end_char;
	//没有指定end_str 或者 不存在end-str
	if ( NULL == end_str || !( end_char = strstr( tmp_result, end_str ) ) ){
		//字符串长度超过 result 接受范围
		if ( strlen( tmp_result ) >= result_len ){
			return NULL;
		}
		strcpy( result, tmp_result );
		return result;
	}
	size_t len = (size_t)( end_char - tmp_result );
	//超过范围
	if ( len >= result_len ){
		return NULL;
	}
	memcpy( result, tmp_result, len );
	result[ len ] = '\0';
	return result;
}