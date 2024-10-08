# pkg-cofnig variable
if(NOT PKGCFG_PREFIX)
    set(PKGCFG_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

if(NOT PKGCFG_NAME)
    set(PKGCFG_NAME ${PROJECT_NAME})
endif()

if(NOT PKGCFG_VERSION)
    set(PKGCFG_VERSION ${PROJECT_VERSION})
endif()

if(NOT PKGCFG_DESCRIPTION)
    set(PKGCFG_DESCRIPTION ${PROJECT_DESCRIPTION})
endif()

if(NOT PKGCFG_HOMEPAGE_URL)
    set(PKGCFG_HOMEPAGE_URL ${PROJECT_HOMEPAGE_URL})
endif()

if(NOT PKGCFG_LIBNAME)
    set(PKGCFG_LIBNAME ${PROJECT_NAME})
endif()