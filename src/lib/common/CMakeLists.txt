#########################################################################
if(WIN32)
    set(BUILD_PROJECT_NAME "libcommon")
elseif(UNIX)
    set(BUILD_PROJECT_NAME "common")
endif()
message(STATUS "Project: ${BUILD_PROJECT_NAME}")

#########################################################################
message(STATUS "Print the include directores")
get_property(inc_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(inc_dir ${inc_dirs})
  message(STATUS "    include directory='${inc_dir}'")
endforeach()

#########################################################################
message(STATUS "Print the link directores")
get_property(link_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
foreach(link_dir ${link_dirs})
  message(STATUS "    link directory='${link_dir}'")
endforeach()

#########################################################################
set(SRC_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/global.h
  ${CMAKE_CURRENT_SOURCE_DIR}/global.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/noncopyable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sigslot.h
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/event_loop.h
  ${CMAKE_CURRENT_SOURCE_DIR}/event_loop.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/event_timer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/event_timer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signal_handler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signal_handler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/listener.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/listener.h
  ${CMAKE_CURRENT_SOURCE_DIR}/connector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/connector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/inet_addr.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inet_addr.h
  ${CMAKE_CURRENT_SOURCE_DIR}/data_buffer_pool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/data_buffer_pool.h
  ${CMAKE_CURRENT_SOURCE_DIR}/async_packet_socket.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/async_packet_socket.h
  ${CMAKE_CURRENT_SOURCE_DIR}/p2p_client.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/p2p_client.h
  ${CMAKE_CURRENT_SOURCE_DIR}/p2p_server.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/p2p_server.h)

#########################################################################
add_library(${BUILD_PROJECT_NAME} STATIC ${SRC_LIST})

