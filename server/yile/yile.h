#ifndef YILE_BASE_HEAD_FILE
#define YILE_BASE_HEAD_FILE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u_char;

#define YILE_OK 0
#define	YILE_ERROR -1

//设置非阻塞
#define yile_nonblocking( s ) fcntl( s, F_SETFL, fcntl( s, F_GETFL ) | O_NONBLOCK )
#endif