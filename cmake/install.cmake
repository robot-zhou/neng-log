if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /neng CACHE PATH "" FORCE)
endif()

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

message("CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")