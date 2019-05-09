#########################################################################
set(BUILD_PROJECT_NAME "server")
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
#Step 3 : Add code source
set(SRC_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

#########################################################################
add_executable(${BUILD_PROJECT_NAME} ${SRC_LIST})
#IF(UNIX)
#	IF(CMAKE_BUILD_TYPE MATCHES Release)
#		MESSAGE(STATUS "stp_centerserver STRIP")
#		add_custom_command(TARGET ${BUILD_PROJECT_NAME} POST_BUILD 
#			COMMAND ${CMAKE_STRIP} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_PROJECT_NAME}
#			COMMAND cp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BUILD_PROJECT_NAME} ${FTP_DIR}/${BUILD_PROJECT_NAME})
#	ENDIF()
#ENDIF()

#########################################################################
if(WIN32)
  target_link_libraries(${BUILD_PROJECT_NAME}
    ${VZPROJECT_LINK_LIB}
  )
else()
  target_link_libraries(${BUILD_PROJECT_NAME}
    common
    jsoncpp
    log4cplus
    event_core
    ${VZPROJECT_LINK_LIB}
  )
endif()
