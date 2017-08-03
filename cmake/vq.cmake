 
file( TO_CMAKE_PATH "$ENV{VQ_ROOT}" VQ_ROOT )

set( PROJECT_ROOT ${VQ_ROOT} )

#set( VQ_BUILD_DIR       ${PROJECT_ROOT}/builds )
#set( VQ_OUTPUT_DIR      ${PROJECT_ROOT}/output )
#set( VQ_SRC_DIR         ${PROJECT_ROOT}/source )
#set( VQ_TEST_DIR        ${PROJECT_ROOT}/tests )

include( ${PROJECT_ROOT}/cmake/cmake_lib/core.cmake)

macro( vq_add_core )
    vq_add_module( "core" )
endmacro()
