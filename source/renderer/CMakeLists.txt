include(${PROJECT_SOURCE_DIR}/cmake/base.cmake)

set(TARGET_NAME SirionRenderer)

set(CURR_PROJECT_SOURCE_DIR "${SIRION_ROOT_DIR}/renderer")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")


include_directories("${VULKAN_PATH}/Include")
include_directories("${SIRION_EXTERNAL_DIR}/glfw/include")
include_directories("${SIRION_EXTERNAL_DIR}/json11")
include_directories("${SIRION_EXTERNAL_DIR}/eigen")
include_directories("${SIRION_EXTERNAL_DIR}/imgui")
include_directories("${SIRION_EXTERNAL_DIR}/glm")
include_directories("${SIRION_EXTERNAL_DIR}/tinyobjectloader")
include_directories("${SIRION_ROOT_DIR}")
source_group(TREE ${CURR_PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})
add_library(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} glfw ${Vulkan_LIBRARY} json11)
add_dependencies(${TARGET_NAME} SirionMath SirionShader SirionParticles)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
endif()