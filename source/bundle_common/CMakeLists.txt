project( bundle_common )
cmake_minimum_required(VERSION 3.1)
cmake_policy( SET CMP0020 NEW )
include( $ENV{VQ_ROOT}/cmake/vq.cmake )

# This is a static lib that can be linked with a plugin. This provides basic
# functionality and preserves convention
# This library SHOULT NOT be linked with non-plugin projects

vq_add_core()


vq_classes(
    BundleContext
    AppContext
)

vq_headers(
    BundleLogging.h
)


vq_create_staticlib()
vq_install()

