cmake_minimum_required (VERSION 3.8)

project ("Sirion" VERSION 0.1.0)

# General settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
include(CMakeDependentOption)

# Vulkan
set(VULKAN_PATH "C:/VulkanSDK/1.3.231.1")

# Include sub-projects.
add_subdirectory ("source")
