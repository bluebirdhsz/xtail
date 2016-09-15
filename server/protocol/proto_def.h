#ifndef PROTOCOL_POOL_SIZE_HEAD
#define PROTOCOL_POOL_SIZE_HEAD
//加入结果 解析时占内存
#define PROTO_SIZE_JOIN_RESULT 16384
//ping结果 解析时占内存
#define PROTO_SIZE_PING_RE 16384
//加入服务器 解析时占内存
#define PROTO_SIZE_JOIN_XTAIL 16384
//ping 解析时占内存
#define PROTO_SIZE_PING sizeof( proto_ping_t ) + sizeof( packet_head_t )
//推送数据 解析时占内存
#define PROTO_SIZE_PUSH_MSG 16384
#define PACK_ID_JOIN_RESULT 102
#define PACK_ID_PING_RE 104
#define PACK_ID_JOIN_XTAIL 101
#define PACK_ID_PING 103
#define PACK_ID_PUSH_MSG 103
#endif
