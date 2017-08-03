project( vq )
cmake_minimum_required(VERSION 3.1)
cmake_policy( SET CMP0020 NEW )
set( PROJECT_ROOT $ENV{VQ_ROOT} )
include( $ENV{VQ_ROOT}/cmake/cmake_lib/container.cmake )

if(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

add_project( source )
add_project( plugins )
add_project( tests )
