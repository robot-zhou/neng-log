# build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# check compile type
message("C Compiler: " ${CMAKE_C_COMPILER_ID})
message("C Compiler Version: " ${CMAKE_C_COMPILER_VERSION})
message("C++ Compiler: " ${CMAKE_CXX_COMPILER_ID})
message("C++ Compiler Version: " ${CMAKE_CXX_COMPILER_VERSION})

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CLANG 1)
elseif("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xGNU")
    set(GNU 1)
else()
    message(FATAL_ERROR "not support compiler")
endif()

# check os/machineos/machine
include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
message("System: " ${CMAKE_SYSTEM_NAME} ", CPU: " ${CMAKE_SYSTEM_PROCESSOR} ", Big Endian: " ${IS_BIG_ENDIAN})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(OS_LINUX 1)

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(CPU_x86_64 1)
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        set(CPU_ARM64 1)
    else()
        message(FATAL_ERROR "not support cpu")
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(OS_DARWIN 1)

    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(CPU_x86_64 1)
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
        set(CPU_ARM64 1)
    else()
        message(FATAL_ERROR "not support cpu")
    endif()
else()
    message(FATAL_ERROR "not support os")
endif()

# debug/relase version macro
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_definitions(DEBUG _DEBUG) # default cmake add NDEBUG macro for Release, RelWithDebInfo
endif()

# os type
if(OS_LINUX)
    add_compile_definitions(LINUX __LINUX__)
endif()

if(OS_DARWIN)
    add_compile_definitions(DARWIN __DARWIN__)
endif()

# cpu type
if(CPU_x86_64)
    add_compile_definitions(__x86_64__ __amd64__)
endif()

if(CPU_ARM64)
    add_compile_definitions(__arm64__ __aarch64__)
endif()

# byte endian
if(IS_BIG_ENDIAN)
    add_compile_definitions(__BIG_ENDIAN__)
else()
    add_compile_definitions(__LITTLE_ENDIAN__)
endif()

# compile/link options
set(COMMON_COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fno-common -Wall -Werror -Wno-unused-parameter -Wredundant-decls -Wpointer-arith -Wcast-qual -Wundef -Wno-builtin-macro-redefined")
set(COMMON_COMPILE_CXX_FLAGS "${COMMON_COMPILE_FLAGS}")
set(COMMON_COMPILE_C_FLAGS "${COMMON_COMPILE_FLAGS} -Wno-pointer-sign -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes")

set(CMAKE_CXX_STANDARD 14)
add_link_options(-Wl,-Bsymbolic -Wl,--no-undefined)

# 重新定义当前目标的源文件的__FILE__宏
function(redefine_file_macro targetname)
    # 获取当前目标的所有源文件
    get_target_property(source_files "${targetname}" SOURCES)

    # 遍历源文件
    foreach(sourcefile ${source_files})
        # 获取当前源文件的编译参数
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)

        # 获取当前文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)

        # 将绝对路径中的项目路径替换成空,得到源文件相对于项目路径的相对路径
        string(REPLACE ${PROJECT_SOURCE_DIR}/ "" relpath ${filepath})

        # 将我们要加的编译参数(__FILE__定义)添加到原来的编译参数里面
        list(APPEND defs "__FILE__=\"${relpath}\"")

        # 重新设置源文件的编译参数
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs}
        )
    endforeach()
endfunction()