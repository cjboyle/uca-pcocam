

set(PCOSDK_INCLUDE_DIRS)
set(PCOSDK_LIBRARIES)
set(PCOSDK_DEFINITIONS)

include(ClSerSis)
include(FgLib5)
include(Haprt)

if(WIN32)
    
else()
    set(PCOSDK_PREFIX "/opt/PCO" CACHE PATH "PCO.Linux SDK location")
    set(PCOSDK_LIB_DIRS)

    list(APPEND PCOSDK_INCLUDE_DIRS
        "${PCOSDK_PREFIX}/pco_common/pco_include"
        "${PCOSDK_PREFIX}/pco_common/pco_classes"
        "${PCOSDK_PREFIX}/pco_clhs/pco_clhs_common"
        "${PCOSDK_PREFIX}/pco_clhs/pco_classes"
    )

    list(APPEND PCOSDK_LIB_DIRS
        "${PCOSDK_PREFIX}/pco_clhs/bindyn"
        "${PCOSDK_PREFIX}/pco_common/pco_lib"
        "/usr/lib"
        "/usr/lib64"
        "/usr/local/lib"
        "/usr/local/lib64"
    )

    set(PCO_CAM_CLHS)
    set(PCO_CLHS)
    set(PCO_FILE)
    set(PCO_LOG)
    set(PCO_REORDERFUNC)
    find_library(PCO_CAM_CLHS pcocam_clhs HINTS ${PCOSDK_LIBRARIES})
    find_library(PCO_CLHS pcoclhs HINTS ${PCOSDK_LIBRARIES})
    find_library(PCO_FILE pcofile HINTS ${PCOSDK_LIBRARIES})
    find_library(PCO_LOG pcolog HINTS ${PCOSDK_LIBRARIES})
    find_library(PCO_REORDERFUNC reorderfunc HINTS ${PCOSDK_LIBRARIES})

    set(SISO_FGLIB5)
    set(SISO_CLSERSIS)
    set(SISO_HAPRT)
    find_library(SISO_FGLIB5 fglib5)
    find_library(SISO_FGLIB5 clsersis)
    find_library(SISO_FGLIB5 haprt)

    list(APPEND PCOSDK_LIBRARIES
        PCO_CLHS PCO_CAM_CLHS PCO_FILE PCO_REORDERFUNC
        SISO_FGLIB5 SISO_CLSERSIS SISO_HAPRT
    )

    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(PCOSDK DEFAULT_MSG
        PCOSDK_LIBRARIES
        PCOSDK_INCLUDE_DIRS
    )

    list(APPEND PCOSDK_DEFINITIONS WITH_GIT_VERSION)