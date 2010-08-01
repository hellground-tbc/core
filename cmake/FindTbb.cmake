# This script is taken from FindAce script.
# - Locate the tbb library
# This module defines
#  TBB_FOUND -- true if tbb was found
#  TBB_LIBRARY -- the library to link against
#  TBB_INCLUDE_DIR -- path to tbb/tbb.h
MACRO(FIND_TBB LIBNAME)
    GET_FILENAME_COMPONENT(parent_dir_ "${PROJECT_SOURCE_DIR}/.." ABSOLUTE)
    FIND_PATH(
        ACE_INCLUDE_DIR tbb/tbb.h
        PATHS /usr/include /usr/local/include /opt/intel/tbb /root/tbb /usr/include/tbb /usr/local/include/tbb 
        "${CMAKE_INSTALL_PREFIX}/include" "${parent_dir_}/tbb"
        DOC "Path to tbb/tbb.h"
    )

    FIND_LIBRARY(
        TBB_LIBRARY "${LIBNAME}"
        PATHS /usr/lib /usr/local/lib /usr/lib64
        "${CMAKE_INSTALL_PREFIX}/lib" "${parent_dir_}/tbb/build"
        DOC "Path to tbb libraries"
    )

    FIND_LIBRARY(
        TBB_LIBRARY_MALLOC "${LIBNAME}malloc"
        PATHS /usr/lib /usr/local/lib /usr/lib64
        "${CMAKE_INSTALL_PREFIX}/lib" "${parent_dir_}/tbb/build"
        DOC "Path to tbb libraries"
    )

    FIND_LIBRARY(
        TBB_LIBRARY_MALLOC_PROXY "${LIBNAME}malloc_proxy"
        PATHS /usr/lib /usr/local/lib /usr/lib64
        "${CMAKE_INSTALL_PREFIX}/lib" "${parent_dir_}/tbb/build"
        DOC "Path to tbb libraries"
    )

    IF(TBB_INCLUDE_DIR AND TBB_LIBRARY AND TBB_LIBRARY_MALLOC AND TBB_LIBRARY_MALLOC_PROXY)
        SET(TBB_FOUND TRUE)
    ELSE(TBB_INCLUDE_DIR AND TBB_LIBRARY AND TBB_LIBRARY_MALLOC AND TBB_LIBRARY_MALLOC_PROXY)
        SET(TBB_FOUND FALSE)
    ENDIF(TBB_INCLUDE_DIR AND TBB_LIBRARY AND TBB_LIBRARY_MALLOC AND TBB_LIBRARY_MALLOC_PROXY)
ENDMACRO(FIND_TBB)
