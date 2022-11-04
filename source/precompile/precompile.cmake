# 
set(PRECOMPILE_TOOLS_PATH "${PROJECT_BINARY_DIR}")
set(PRECOMPILE_PARAMS_IN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/precompile/precompile.json.in")
set(PRECOMPILE_PARAMS_PATH "${PRECOMPILE_TOOLS_PATH}/precompile.json")
configure_file(${PRECOMPILE_PARAMS_IN_PATH} ${PRECOMPILE_PARAMS_PATH})

#
# use wine for linux
if (CMAKE_HOST_WIN32)
    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/SirionParser.exe)
    set(sys_include "*") 
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/SirionParser)
    set(sys_include "/usr/include/c++/9/") 
    #execute_process(COMMAND chmod a+x ${PRECOMPILE_PARSER} WORKING_DIRECTORY ${PRECOMPILE_TOOLS_PATH})
elseif(CMAKE_HOST_APPLE)
    find_program(XCRUN_EXECUTABLE xcrun)
    if(NOT XCRUN_EXECUTABLE)
      message(FATAL_ERROR "xcrun not found!!!")
    endif()

    execute_process(
      COMMAND ${XCRUN_EXECUTABLE} --sdk macosx --show-sdk-platform-path
      OUTPUT_VARIABLE osx_sdk_platform_path_test
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/SirionParser)
    set(sys_include "${osx_sdk_platform_path_test}/../../Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1") 
endif()

set (PARSER_INPUT ${CMAKE_BINARY_DIR}/parser_header.h)
set (TEMPLATE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/meta_parser)
### BUILDING ====================================================================================
set(PRECOMPILE_TARGET "SirionPreCompile")

# Called first time when building target 
add_custom_target(${PRECOMPILE_TARGET} ALL

# COMMAND # (DEBUG: DON'T USE )
#     this will make configure_file() is called on each compile
#   ${CMAKE_COMMAND} -E touch ${PRECOMPILE_PARAM_IN_PATH}a

# If more than one COMMAND is specified they will be executed in order...
COMMAND
  ${CMAKE_COMMAND} -E echo "************************************************************* "
COMMAND
  ${CMAKE_COMMAND} -E echo "**** [Precompile] BEGIN "
COMMAND
  ${CMAKE_COMMAND} -E echo "************************************************************* "

COMMAND
    ${PRECOMPILE_PARSER} "${PRECOMPILE_PARAMS_PATH}"  "${PARSER_INPUT}"  "${SIRION_ROOT_DIR}" "${TEMPLATE_ROOT_DIR}" ${sys_include} "Sirion" 0
### BUILDING ====================================================================================
COMMAND
    ${CMAKE_COMMAND} -E echo "+++ Precompile finished +++"
)
