cmake_minimum_required (VERSION 3.8)

project ("Sirion" VERSION 0.1.0)

# General settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(CMakeDependentOption)

# 3rd-party settings
set(GLSLANG_VALIDATOR_PATH "C:/VulkanSDK/1.3.216.0/Bin/glslangValidator.exe")

# Include sub-projects.
add_subdirectory ("source")
