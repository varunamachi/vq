project( vqcore )
cmake_minimum_required(VERSION 3.1)
cmake_policy( SET CMP0020 NEW )
include( $ENV{VQ_ROOT}/cmake/vq.cmake )

add_definitions( -DVQ_CORE )

vq_headers(
    Vq.h
)

add_subdirectory( common )
add_subdirectory( logger )
add_subdirectory( job_handler )
add_subdirectory( config_storage )
add_subdirectory( plugin_manager )
add_subdirectory( message_center )
add_subdirectory( platform )

vq_create_sharedlib()
vq_install()

message( STATUS  )
