set(VZPROJECT_INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/src/lib
  ${PROJECT_SOURCE_DIR}/src/third_part
  ${PROJECT_SOURCE_DIR}/src/third_part/libevent-2.1.8/ubuntu64/include
  ${PROJECT_SOURCE_DIR}/src/third_part/log4cplus-2.0.4/ubuntu64/include
  ${PROJECT_SOURCE_DIR}/src/third_part/jsoncpp-1.8.4/ubuntu64/include
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(VZPROJECT_LIBRARY_DIR
    ${PROJECT_SOURCE_DIR}/lib/ubuntu64/Debug
    ${PROJECT_SOURCE_DIR}/src/third_part/libevent-2.1.8/ubuntu64/lib
    ${PROJECT_SOURCE_DIR}/src/third_part/log4cplus-2.0.4/ubuntu64/lib
    ${PROJECT_SOURCE_DIR}/src/third_part/jsoncpp-1.8.4/ubuntu64/lib
  )

  set(VZPROJECT_LINK_LIB
    -g
    -Wl,--rpath=.,--enable-new-dtags
    pthread
    rt
    dl
  )

  add_definitions(
  )
else(CMAKE_BUILD_TYPE MATCHES Release)
  set(VZPROJECT_LIBRARY_DIR
    ${PROJECT_SOURCE_DIR}/lib/ubuntu64/Release
    ${PROJECT_SOURCE_DIR}/src/third_part/libevent-2.1.8/ubuntu64/lib
    ${PROJECT_SOURCE_DIR}/src/third_part/log4cplus-2.0.4/ubuntu64/lib
    ${PROJECT_SOURCE_DIR}/src/third_part/jsoncpp-1.8.4/ubuntu64/lib
  )

  set(VZPROJECT_LINK_LIB
    -Wl,--rpath=.,--enable-new-dtags
    pthread
    rt
    dl
  )

  add_definitions(
  )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/bin/ubuntu64")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/lib/ubuntu64")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/lib/ubuntu64")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/bin/ubuntu64/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/lib/ubuntu64/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/lib/ubuntu64/Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/bin/ubuntu64/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/lib/ubuntu64/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/lib/ubuntu64/Release")


