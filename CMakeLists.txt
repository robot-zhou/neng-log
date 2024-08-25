cmake_minimum_required(VERSION 3.0)
project(neng-log VERSION 1.0.0
    DESCRIPTION "neng log library"
    HOMEPAGE_URL "https://gitee.com/robot-zhou/neng-log")

# include cmake files
set(CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")
include(${CMAKE_DIR}/general.cmake)
include(${CMAKE_DIR}/install.cmake)
include(${CMAKE_DIR}/pkgcfg.cmake)

# project variables
if(NOT TOP_DIR)
    set(TOP_DIR "${CMAKE_SOURCE_DIR}")
endif()

set(OUTPUT_DIR "${TOP_DIR}")
set(OUTPUT_BIN_DIR "${OUTPUT_DIR}/bin")
set(OUTPUT_LIB_DIR "${OUTPUT_DIR}/lib")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_LIB_DIR})

set(INC_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/test")
set(3RDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdparty")
set(PROJECT_SOVERSION 1.0)

# common compile options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_COMPILE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_CXX_FLAGS}")

# common include directory
include_directories(${INC_DIR})
include_directories(${3RDPARTY_DIR})

# commin library directory
link_directories(${OUTPUT_LIB_DIR})

# libneng-log source files
set(NENG_LOG_SRC_DIR ${SRC_DIR}/neng/log)
file(GLOB_RECURSE NENG_LOG_SRC_FILES ${NENG_LOG_SRC_DIR}/*.cpp ${NENG_LOG_SRC_DIR}/*.c)

# TARGET: libneng-log static
add_library(${PROJECT_NAME}-static STATIC ${NENG_LOG_SRC_FILES})
redefine_file_macro(${PROJECT_NAME}-static)
set_target_properties(${PROJECT_NAME}-static PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
)

# TARGET: libneng-log shared
add_library(${PROJECT_NAME}-shared SHARED ${NENG_LOG_SRC_FILES})
redefine_file_macro(${PROJECT_NAME}-shared)
strip_file(${PROJECT_NAME}-shared)
target_link_libraries(${PROJECT_NAME}-shared pthread)
set_target_properties(${PROJECT_NAME}-shared PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION}
)

# TARGET: test example
set(TEST_EXAMPLE_SRC_DIR ${TEST_DIR}/example)
file(GLOB_RECURSE TEST_EXAMPLE_SRC_FILES ${TEST_EXAMPLE_SRC_DIR}/*.cpp ${TEST_EXAMPLE_SRC_DIR}/*.c)
add_executable(test-${PROJECT_NAME}-example ${TEST_EXAMPLE_SRC_FILES})
redefine_file_macro(test-${PROJECT_NAME}-example)
target_link_libraries(test-${PROJECT_NAME}-example ${PROJECT_NAME} pthread)

# TARGET: test example-properties
set(TEST_EXAMPLE_PROPERTIES_SRC_DIR ${TEST_DIR}/example-properties)
file(GLOB_RECURSE TEST_EXAMPLE_PROPERTIES_SRC_FILES ${TEST_EXAMPLE_PROPERTIES_SRC_DIR}/*.cpp ${TEST_EXAMPLE_PROPERTIES_SRC_DIR}/*.c)
add_executable(test-${PROJECT_NAME}-example-properties ${TEST_EXAMPLE_PROPERTIES_SRC_FILES})
redefine_file_macro(test-${PROJECT_NAME}-example-properties)
target_link_libraries(test-${PROJECT_NAME}-example-properties ${PROJECT_NAME} pthread)

# DEPENDENCY
add_dependencies(test-${PROJECT_NAME}-example ${PROJECT_NAME}-static ${PROJECT_NAME}-shared)
add_dependencies(test-${PROJECT_NAME}-example-properties ${PROJECT_NAME}-static ${PROJECT_NAME}-shared)

# CONFIGURE Files
configure_file(${PROJECT_SOURCE_DIR}/neng-log.pc.in ${OUTPUT_LIB_DIR}/pkgconfig/neng-log.pc @ONLY NEWLINE_STYLE UNIX)

# INSTALL
install(TARGETS test-${PROJECT_NAME}-example test-${PROJECT_NAME}-example-properties DESTINATION bin)
install(TARGETS ${PROJECT_NAME}-static ${PROJECT_NAME}-shared DESTINATION lib)
install(DIRECTORY ${INC_DIR}/neng DESTINATION include)
install(FILES ${OUTPUT_LIB_DIR}/pkgconfig/neng-log.pc DESTINATION lib/pkgconfig)

if(OS_LINUX AND GNU)
    install(FILES $<TARGET_FILE:${PROJECT_NAME}-shared>.debug DESTINATION lib)
endif()
