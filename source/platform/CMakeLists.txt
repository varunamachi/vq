
include( ${PROJECT_ROOT}/cmake/cmake_lib/platform.cmake )

add_subdirectory( file_system )
add_subdirectory( shared_library )

vq_headers(
    Platform.h
)

