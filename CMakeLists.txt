cmake_minimum_required(VERSION 2.8)

project(EVHANDLER)

# configuration
include(cmake/config.cmake)
include(cmake/macros.cmake)

if(DEFINED ENV{EVHANDLER_HOME})
    set(CMAKE_INSTALL_PREFIX $ENV{EVHANDLER_HOME}/install)
else()
	set(CMAKE_INSTALL_PREFIX ${EVHANDLER_BINARY_DIR}/install)
endif()
message(STATUS "Install Prefix is ${CMAKE_INSTALL_PREFIX}")

#set(COMMON_LIBS common)

# complie options
#add_definitions(-D_LOG_ON_MEMORY)

add_compile_flags("-pipe -Wall -Werror=implicit-function-declaration")
add_compile_flags("-Wdeclaration-after-statement -fno-strict-aliasing")
add_compile_flags("-gdwarf-2 -fno-omit-frame-pointer")


if(CFG_DEBUG)
    add_compile_flags("-g")
else()
    add_compile_flags("-O3")
endif()    

if(ARCHFLAGS STREQUAL "m32")
    add_compile_flags("-m32")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(CFG_LINUX ON)
endif()

#configure_file(${EVHANDLER_SOURCE_DIR}/include/config.h.in
#   			   ${EVHANDLER_BINARY_DIR}/include/config.h)
#add_compile_flags("-include ${EVHANDLER_BINARY_DIR}/include/config.h")

include_directories(${EVHANDLER_SOURCE_DIR}/include)
include_directories(${EVHANDLER_BINARY_DIR}/include)

add_subdirectory(main)
add_subdirectory(evhandler)
#add_subdirectory(sandbox)


add_custom_target(clean-all
                  COMMAND ${CMAKE_BUILD_TOOL} clean
                  COMMAND find . \\\( -name \\*.[oa] -o -name \\*.swp -o -name
                  \\*.core \\\) -type f -print | xargs rm -f
                  COMMAND rm -rf TAGS tags cscope.* 
                 )

add_custom_target(tags
                  COMMAND find . -name "*.[chSsly]" -a -type f > cscope.files
	          	  COMMAND cscope -b
	          	  COMMAND xargs exctags -e --c-kinds=+x < cscope.files
	          	  COMMAND exctags -R --c-kinds=+x --links=no *
                 )
