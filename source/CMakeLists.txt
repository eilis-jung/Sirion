# Project settings
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(PROJECT_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/libs)
set(SIRION_ROOT_DIR "${PROJECT_SOURCE_DIR}/source")
set(SIRION_EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/source/external")
set(SIRION_ASSET_DIR "${PROJECT_SOURCE_DIR}/asset")

# For MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SirionMain)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# Vulkan
set(GLSLANG_VALIDATOR_PATH "${VULKAN_PATH}/Bin/glslangValidator.exe")
set(WIN_VULKAN_LIB "${VULKAN_PATH}/Lib")
IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${WIN_VULKAN_LIB})
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Found Vulkan!")
		ENDIF()
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ENDIF()

# Add include for 3rd-party dependencies
include_directories("${VULKAN_PATH}/Include/vulkan")
include_directories("${VULKAN_PATH}/Include/vk_video")
include_directories("${SIRION_EXTERNAL_DIR}/glfw/include")
include_directories("${SIRION_EXTERNAL_DIR}/json11")
include_directories("${SIRION_EXTERNAL_DIR}/eigen")
include_directories("${SIRION_EXTERNAL_DIR}/imgui")

# Sub-projects
add_subdirectory ("external")
add_subdirectory ("shader")
add_subdirectory ("math")
add_subdirectory ("particles")
add_subdirectory ("physics")
add_subdirectory ("renderer")
add_subdirectory ("simulation")
add_subdirectory ("solver")
add_subdirectory ("main")
add_subdirectory ("meta")