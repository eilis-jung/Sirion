include(${PROJECT_SOURCE_DIR}/cmake/base.cmake)

set(TARGET_NAME SirionMath)

set(CURR_PROJECT_SOURCE_DIR "${SIRION_ROOT_DIR}/math")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

include_directories("${VULKAN_PATH}/Include")
include_directories("${SIRION_EXTERNAL_DIR}/eigen")
include_directories("${SIRION_EXTERNAL_DIR}/glm")
include_directories("${SIRION_EXTERNAL_DIR}/stb")
include_directories("${SIRION_ROOT_DIR}")
source_group(TREE ${CURR_PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})
add_library(${TARGET_NAME} ${SOURCES} ${HEADERS})
# target_link_libraries(${TARGET_NAME} glfw ${Vulkan_LIBRARY} json11)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
endif()