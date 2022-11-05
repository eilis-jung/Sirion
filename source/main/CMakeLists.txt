include(${PROJECT_SOURCE_DIR}/cmake/base.cmake)

set(TARGET_NAME SirionMain)

set(CURR_PROJECT_SOURCE_DIR "${SIRION_ROOT_DIR}/main")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# Build 3rd-party tools
file(GLOB GLFW_LIBRARY ${PROJECT_LIBRARY_DIR}/glfw/glfw3.lib)

source_group(TREE ${CURR_PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
add_dependencies(${TARGET_NAME} glfw json11 )
add_dependencies(${TARGET_NAME} SirionPrecompile SirionShader SirionRenderer)
target_link_libraries(${TARGET_NAME} glfw ${Vulkan_LIBRARY} json11)
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${SIRION_ROOT_DIR}>)

# set global var used by precompile
set(SIRION_MAIN_HEADERS "${HEADERS}" PARENT_SCOPE)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
endif()