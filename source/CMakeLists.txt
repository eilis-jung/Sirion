# Project settings
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/libs)

# Project settings
set(SIRION_ROOT_DIR "${PROJECT_SOURCE_DIR}/source")
set(SIRION_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")
set(SIRION_ASSET_DIR "${PROJECT_SOURCE_DIR}/asset")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SirionMain)
endif()

set(SHADER_COMPILE_TARGET SirionShader)

# Add include for 3rd-party dependencies
include_directories("${SIRION_ROOT_DIR}/external/glfw/include")
include_directories("${SIRION_ROOT_DIR}/external/freeglut/include")
include_directories("${SIRION_ROOT_DIR}/external/json11")
include_directories("${SIRION_ROOT_DIR}/external/eigen")


add_subdirectory ("external")
add_subdirectory ("shader")
add_subdirectory ("particle_system")
add_subdirectory ("physics")
add_subdirectory ("renderer")
add_subdirectory ("simulation")
add_subdirectory ("solver")
add_subdirectory ("main")