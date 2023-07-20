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
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_C_FLAGS} -Wno-nonnull")
if(${GNU})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-overflow")
endif()
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

# test example target
set(TEST_EXAMPLE_SRC_DIR ${TEST_DIR}/example)
file(GLOB_RECURSE TEST_EXAMPLE_SRC_FILES ${TEST_EXAMPLE_SRC_DIR}/*.cpp ${TEST_EXAMPLE_SRC_DIR}/*.c)
add_executable(example ${TEST_EXAMPLE_SRC_FILES})
target_link_libraries(example neng-log pthread)
set_target_properties(example PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR} 
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR} 
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_CONF_DIR} 
)
add_dependencies(example neng-log)

# test example-properties target
set(TEST_EXAMPLE_PROPERTIES_SRC_DIR ${TEST_DIR}/example-properties)
file(GLOB_RECURSE TEST_EXAMPLE_PROPERTIES_SRC_FILES ${TEST_EXAMPLE_PROPERTIES_SRC_DIR}/*.cpp ${TEST_EXAMPLE_PROPERTIES_SRC_DIR}/*.c)
add_executable(example-properties ${TEST_EXAMPLE_PROPERTIES_SRC_FILES})
target_link_libraries(example-properties neng-log pthread)
set_target_properties(example-properties PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR} 
    ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR} 
    LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_CONF_DIR} 
)
add_dependencies(example-properties neng-log)
