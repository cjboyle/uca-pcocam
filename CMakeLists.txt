cmake_minimum_required(VERSION 2.8)
project(ucapcoclhs C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}"/cmake)

find_package(PkgConfig)
include(PkgConfigVars)

add_definitions("-std=c99 -Wall -fPIC")
add_definitions(-DG_LOG_DOMAIN="uca-pcoclhs")

#pkg_check_modules(UCA libuca>=2.1.0 REQUIRED)
#pkg_check_variable(libuca plugindir)


# --- Plugin specific --------------------------------

find_package(FgLib5)
find_package(ClSerSis)
find_package(PcoSdk)

include(Glib2MkEnums)

include_directories(
    ${clsersis_INCLUDE_DIR}
    ${FgLib5_INCLDUE_DIR}
    ${PcoSdk_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${UCA_INCLUDE_DIRS}
)

glib2_mkenums(uca-pcoclhs-enums
    uca-pcoclhs-enums
    uca-pcoclhs-enums.h
)

link_directories(
    ${UCA_LIBRARY_DIRS}
)

add_library(ucapcoclhs SHARED
    uca-pcoclhs-camera.c
    uca-pco-enums.c
)

target_link_libraries(ucapcoclhs
    ${UCA_LIBRARIES}
    ${clsersis_LIBRARY}
    ${FgLib5_LIBRARY}
    ${PcoSdk_LIBRARY}
)

install(TARGETS ucapcoclhs
    LIBRARY DESTINATION ${LIBUCA_PLUGINDIR}
)


set(VERSION 1.0.0)
set(RELEASE 1)