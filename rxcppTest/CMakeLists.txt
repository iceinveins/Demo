# CMakeList.txt : CMake project for OverNightFeatures, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project("RxcppTest")

# 禁用 RPATH
# set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

function(color_message COLOR MSG)
    string(ASCII 27 Esc)
    set(RESET "${Esc}[0m")
    
    if("${COLOR}" STREQUAL "RED")
        set(COLOR_CODE "${Esc}[31m")
    elseif("${COLOR}" STREQUAL "GREEN")
        set(COLOR_CODE "${Esc}[32m")
    # ... 其他颜色
    endif()
    
    message("${COLOR_CODE}${MSG}${RESET}")
endfunction()

set(CXX_FLAGS
# -DVALGRIND
# -DCHECK_ENABLE
# -DMEMORY_OVERLAP_CHECK
# -DFACTOR_INSIDE_TIME_STAT
# -DTD4_MERGE_SORT
-DPLT2_MERGE_SORT
-DCHECK_PTHREAD_RETURN_VALUE
-D_FILE_OFFSET_BITS=64
-pthread
-Wall
-Wextra
-Werror
-Wno-unused-parameter
-Wno-null-dereference
#-Wconversion
-Wuninitialized
-Winit-self 
-Wmissing-include-dirs
#-Wold-style-cast
-Woverloaded-virtual
-Wpointer-arith
-Wshadow
-Wwrite-strings
# -fno-gnu-unique
# -Wthread-safety
-fstack-protector-all
-fPIC
# -MMD
-std=c++20
# -rdynamic
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DTEST_QUOTE_TIME) #测试模拟柜台时更改发单时间
    list(APPEND CXX_FLAGS "-fsanitize=address")
    list(APPEND CXX_FLAGS "-fsanitize=undefined")
    list(APPEND CXX_FLAGS "-fno-omit-frame-pointer")
    list(APPEND CXX_FLAGS "-fsanitize-address-use-after-scope")
    list(APPEND CXX_FLAGS "-g3")
    list(APPEND CXX_FLAGS "-fno-optimize-sibling-calls")
    list(APPEND CXX_FLAGS "-fstandalone-debug")
    list(APPEND CXX_FLAGS "-fprofile-instr-generate")
    list(APPEND CXX_FLAGS "-fcoverage-mapping")
    color_message("RED" "Debug模式: 已启用sanitizer")
else()
    set(CMAKE_BUILD_TYPE "Release") #CMAKE_BUILD_TYPE不填默认Release
    list(APPEND CXX_FLAGS "-O3")
    list(APPEND CXX_FLAGS "-g3")
    list(APPEND CXX_FLAGS "-march=native")
    color_message("GREEN" "Release模式: 未启用sanitizer")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/)

# find_package(Boost REQUIRED COMPONENTS filesystem system locale)
# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     link_directories(${Boost_LIBRARY_DIRS})
# endif()
include_directories(${PROJECT_SOURCE_DIR})

message(STATUS "CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})

include_directories(${PROJECT_BINARY_DIR})

# Disable Abseil tests, samples, and benchmarks
set(ABSL_ENABLE_TESTING OFF CACHE BOOL "Disable Abseil tests" FORCE)
set(ABSL_BUILD_TESTING OFF CACHE BOOL "Disable Abseil BUILD_TESTING" FORCE)
set(ABSL_BUILD_BENCHMARKS OFF CACHE BOOL "Disable Abseil benchmarks" FORCE)

# add_subdirectory(src)
include_directories(${PROJECT_SOURCE_DIR})

include_directories(
    /usr/include/;
    /usr/local/include/;
)

link_directories(
    /usr/lib/x86_64-linux-gnu/;
)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/logger)

add_executable(rxcppTest main.cpp logger/logger.cpp)
set_target_properties(rxcppTest PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(rxcppTest)

