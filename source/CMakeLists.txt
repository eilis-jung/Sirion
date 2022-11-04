# Project settings
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/libs)

# Project settings
set(SIRION_ROOT_DIR "${PROJECT_SOURCE_DIR}/source")
set(SIRION_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")
set(SIRION_ASSET_DIR "${PROJECT_SOURCE_DIR}/asset")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SirionMain)
endif()

set(SHADER_COMPILE_TARGET SirionShader)

# Vulkan
set(WIN_VULKAN_LIB "C:/VulkanSDK/1.3.216.0/Lib")
include_directories("C:/VulkanSDK/1.3.216.0/Include/vulkan")
include_directories("C:/VulkanSDK/1.3.216.0/Include/vk_video")

IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${WIN_VULKAN_LIB})
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ENDIF()

# Add include for 3rd-party dependencies
include_directories("external/vulkan/include")
include_directories("${SIRION_EXTERNAL_DIR}/glfw/include")
include_directories("${SIRION_EXTERNAL_DIR}/json11")
include_directories("${SIRION_EXTERNAL_DIR}/eigen")
include_directories("${SIRION_EXTERNAL_DIR}/imgui")





add_subdirectory ("external")

add_subdirectory ("shader")
add_subdirectory ("particle_system")
add_subdirectory ("physics")
add_subdirectory ("renderer")
add_subdirectory ("simulation")
add_subdirectory ("solver")
add_subdirectory ("main")
add_subdirectory("meta_parser")

include(${SIRION_ROOT_DIR}/precompile/precompile.cmake)

set(CODEGEN_TARGET "SirionPreCompile")

add_dependencies(SirionMain "${CODEGEN_TARGET}")
add_dependencies("${CODEGEN_TARGET}" "SirionParser")