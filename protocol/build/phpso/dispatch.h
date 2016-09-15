#ifndef PROTO_PHPSO_DISPATCH_H
#define PROTO_PHPSO_DISPATCH_H
#define add_proto_map_pack_func()																	\
{																									\
	add_proto_pack_map( 101, sowrite_join_xtail );													\
	add_proto_pack_map( 103, sowrite_ping );														\
	add_proto_pack_map( 103, sowrite_push_msg );													\
}
#define add_proto_map_unpack_func()																	\
{																									\
	add_proto_unpack_map( 102, soread_join_result );												\
	add_proto_unpack_map( 104, soread_ping_re );													\
}
#endif