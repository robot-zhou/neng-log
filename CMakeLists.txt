cmake_minimum_required(VERSION 3.0)
project(neng-log)

# project variables
set(TOP_DIR "${PROJECT_SOURCE_DIR}")
set(CMAKE_DIR "${TOP_DIR}/cmake")
set(INC_DIR "${TOP_DIR}/include")
set(SRC_DIR "${TOP_DIR}/src")
set(TEST_DIR "${TOP_DIR}/test")
set(3RDPARTY_DIR "${TOP_DIR}/3rdparty")

set(OUTPUT_DIR "${TOP_DIR}/objs")
set(OUTPUT_BIN_DIR "${OUTPUT_DIR}/bin")
set(OUTPUT_LIB_DIR "${OUTPUT_DIR}/lib")
set(OUTPUT_CONF_DIR "${OUTPUT_DIR}/conf")

# include cmake files
include(${CMAKE_DIR}/general.cmake)

# common compile options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_C_FLAGS} -Wno-nonnull -Wno-format-overflow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_CXX_FLAGS}")

# common include directory
include_directories(${INC_DIR})
include_directories(${3RDPARTY_DIR})

# commin library directory
link_directories(${OUTPUT_LIB_DIR})

# libneng-log target
set(NENG_LOG_SRC_DIR ${SRC_DIR}/neng/log)
file(GLOB_RECURSE NENG_LOG_SRC_FILES ${NENG_LOG_SRC_DIR}/*.cpp ${NENG_LOG_SRC_DIR}/*.c)
add_library(neng-log STATIC ${NENG_LOG_SRC_FILES})
redefine_file_macro(neng-log)
set_target_properties(neng-log PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR} 
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR} 
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_CONF_DIR} 
)

# test target
set(TEST_SRC_DIR ${TEST_DIR})
file(GLOB_RECURSE TEST_SRC_FILES ${TEST_SRC_DIR}/*.cpp ${TEST_SRC_DIR}/*.c)
add_executable(test ${TEST_SRC_FILES})
target_link_libraries(test neng-log pthread)
set_target_properties(test PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR} 
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR} 
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_CONF_DIR} 
)
add_dependencies(test neng-log)