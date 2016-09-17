//
// Created by bluebird on 2016-9-16.
//

#ifndef XCONSOLE_UTILS_H
#define XCONSOLE_UTILS_H

#include "yile.h"
/**
 * 从字符串中找到某一段
 */
char *str_search( char *str, char *beg_str, char *end_str, char *result, int result_len );

/**
 * 将IP地址转换成32位数字
 */
uint32_t ip2long( char *ip );

#ifdef WORDS_BIGENDIAN
#  define ntoh64(x) (x)
#  define hton64(x) (x)
#else /* !WORDS_BIGENDIAN */
uint64_t net_byteswap64(uint64_t x);
#  define ntoh64(x) net_byteswap64(x)
#  define hton64(x) net_byteswap64(x)
#endif /* !WORDS_BIGENDIAN */
#endif //XCONSOLE_UTILS_H
