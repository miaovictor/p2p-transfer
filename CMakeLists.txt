# cmake ../.. -G "Visual Studio 10 2015"
# cmake ../.. -G "Visual Studio 12 2013"
# cmake ../.. -DCMAKE_BUILD_PLATFORM=UBUNTU64 -DCMAKE_BUILD_TYPE=Release
cmake_minimum_required(VERSION 2.8)
project(p2p_transfer)

set(CMAKE_CXX_STANDARD 11)

include(cmake/global_environment_settings.cmake)
if(WIN32)
  if(MSVC12)
    include(cmake/win_vs2013_environment_settings.cmake)
  endif()
elseif(UNIX)
  include(cmake/ubuntu64_environment_settings.cmake)
endif()

include_directories(${VZPROJECT_INCLUDE_DIR})
link_directories(${VZPROJECT_LIBRARY_DIR})


add_subdirectory(src/lib/common)
add_subdirectory(src/run/client)
add_subdirectory(src/run/server)
