# -G "Visual Studio 9 2008"
# First: cd in bulids/type/ (eg: cd builds/arm3730/
# Then, use the below command
#build with arm3730:  cmake ../.. -DCMAKE_TOOLCHAIN_FILE=../../arm_make.cmake -DCMAKE_BUILD_TYPE=Release
#build with vs2013 :  cmake ../.. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 12 2013"
#build with vs2008 :  cmake ../.. -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 9 2008"
#bjam --toolset=msvc-9.0 --build-type=complete link=static threading=multi runtime-link=static --with-thread
# http://blog.csdn.net/jwybobo2007/article/details/7242307
# ./b2 link=static threading=multi runtime-link=static --with-thread
# CMAKE_BUILD_PLATFORM=HISI
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

##########################################################
## Step 1 
SET(INTERFACE_INCLUDE_DIR     ${PROJECT_SOURCE_DIR}/src/third_part/interface/msvc)
SET(INTERFACE_LIBRARY_DIR     ${PROJECT_SOURCE_DIR}/src/third_part/interface/msvc/lib)

##1. INCLUDE_DIRECTORES
SET(VZPROJECT_INCLUDE_DIR
	${INTERFACE_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/src/lib
	${PROJECT_SOURCE_DIR}/src/third_part
	${PROJECT_SOURCE_DIR}/src/third_part/VzBaseEvent/src/lib
	${PROJECT_SOURCE_DIR}/src/third_part/VzBaseEvent/src/third_part
	${PROJECT_SOURCE_DIR}/src/third_part/VzBaseEvent/src/third_part/boost
	${PROJECT_SOURCE_DIR}/src/third_part/openssl/vs2013/include
	${PROJECT_SOURCE_DIR}/src/third_part/curl/vs2013/include
)

MESSAGE(STATUS "       Add library path and name")
if(CMAKE_BUILD_TYPE MATCHES Debug)
	MESSAGE(STATUS "       Debug Mode")
	MESSAGE(STATUS "       MSVC12")
	SET(VZPROJECT_LIBRARY_DIR
		${PROJECT_SOURCE_DIR}/lib/vs2013
		${GOOGLE_PROTOBUF}/lib/vs2013/Debug
		${AVSERVER_DIR}/lib
		${PROJECT_SOURCE_DIR}/src/third_part/VzBaseEvent/lib/vs2013
		${PROJECT_SOURCE_DIR}/src/third_part/openssl/vs2013/lib
		${PROJECT_SOURCE_DIR}/src/third_part/curl/vs2013/lib
		)
	SET(VZPROJECT_LINK_LIB
		ws2_32.lib
		)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	MESSAGE(STATUS "       Release Mode")
	MESSAGE(STATUS "       MSVC12")
	SET(VZPROJECT_LIBRARY_DIR
		${PROJECT_SOURCE_DIR}/src/third_part/VzBaseEvent/lib/vs2013
		${PROJECT_SOURCE_DIR}/src/third_part/openssl/vs2013/lib
		${PROJECT_SOURCE_DIR}/src/third_part/curl/vs2013/lib
		)
	SET(VZPROJECT_LINK_LIB
		ws2_32.lib
		)
endif()

MESSAGE(STATUS "Step 4 : Add code source")
ADD_DEFINITIONS(-DGOOGLE_GLOG_DLL_DECL=
	-DGLOG_NO_ABBREVIATED_SEVERITIES
	-D_CRT_SECURE_NO_WARNINGS
	-DWIN32_LEAN_AND_MEAN
	-D_WINSOCK_DEPRECATED_NO_WARNINGS
	-D_WIN32_WINNT=0x0502
	-D_SCL_SECURE_NO_WARNINGS
	-DOLD_OPENSDK_ENABLE
    -DHAVE_GLOG
	-DMG_ENABLE_HTTP_STREAMING_MULTIPART
)

#####################################################################
# Step 3 :Set visual studio runtime type
set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  string(REPLACE "/MDd" "/MTd" ${CompilerFlag} "${${CompilerFlag}}")
  #string(REPLACE "/EDITANDCONTINUE" "/SAFESEH" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/bin/vs2013")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/lib/vs2013")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROJECT_SOURCE_DIR}/lib/vs2013")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/bin/vs2013")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/lib/vs2013")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG  "${PROJECT_SOURCE_DIR}/lib/vs2013")

# With Release properties
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/bin/vs2013")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/lib/vs2013")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  "${PROJECT_SOURCE_DIR}/lib/vs2013")