project( source )
cmake_minimum_required(VERSION 3.1)
cmake_policy( SET CMP0020 NEW )
set( PROJECT_ROOT $ENV{VQ_ROOT} )
include( $ENV{VQ_ROOT}/cmake/cmake_lib/container.cmake )

add_project( core )
add_project( bundle_common )
