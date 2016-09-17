cmake_minimum_required(VERSION 2.8)
project(xconsole)
add_definitions( -g -DYILE_DEBUG )
set(SOURCE_FILES
        xtail.c xtail.h
        common.h
        xtail_server.c xtail_server.h
        xtail_terminal.c xtail_terminal.h
        protocol/proto.c protocol/proto.h
        web_socket.c web_socket.h
        base64.c base64.h sha1.c sha1.h utils.c utils.h terminal_group.c terminal_group.h server_action.c server_action.h)
INCLUDE_DIRECTORIES(
        yile
        yile/yile_log/include
        yile/yile_buf/include
        yile/yile_ini/include
        yile/yile_process/include
        yile/yile_protocol/include
        yile/yile_net/include
        protocol/
)
LINK_DIRECTORIES(
        yile/yile_buf/lib
        yile/yile_process/lib
        yile/yile_ini/lib
        yile/yile_protocol/lib
        yile/yile_log/lib
        yile/yile_net/lib
)
LINK_LIBRARIES(
        yileini_debug
        yileprocess_debug
        yilelog_debug
        yilebuf_debug
        yilenet_debug
        yileprotocol_debug
)
add_executable(xtail ${SOURCE_FILES})