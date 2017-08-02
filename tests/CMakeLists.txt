project( plugins )
cmake_minimum_required( VERSION 3.1 )
cmake_policy( SET CMP0020 NEW )
include( $ENV{QUARTZ_ROOT}/cmake/cmake_lib/container.cmake )

#TODO this is a hack for now to fix build errors
#add_definitions( -DQUARTZ_CORE )

file( GLOB DIRS RELEATIVE
        RELEATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/* )
foreach( TESTS_DIR ${DIRS} )
    if( IS_DIRECTORY "${TESTS_DIR}" )
        if( EXISTS "${TESTS_DIR}/CMakeLists.txt" )
            if( NOT ${TESTS_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
                message( STATUS "Adding test at ${TESTS_DIR}" )
                add_project( "${TESTS_DIR}" )
            endif()
        endif()
    endif()
endforeach()        
        
        
