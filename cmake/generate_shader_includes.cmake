####################################################################################################
# This function add all the input files into a header file including them.
# Example:
# - input file: data.dat
# - output file: data.h
# - path from the output header to the input shader header files: root_path
# - data length: sizeof(DATA)
# embed_resource("data.dat" "data.h" "DATA")
####################################################################################################

function(embed_resource resource_file_names source_file_name root_path)

    if(EXISTS "${source_file_name}")
        if("${source_file_name}" IS_NEWER_THAN "${resource_file_name}")
            return()
        endif()
    endif()

    foreach(resource_file ${resource_file_names})
        if(EXISTS "${resource_file}")
            set(source resource_file)
            file(WRITE "${source_file_name}" "${source}")
        else()
            message("ERROR: ${resource_file} doesn't exist")
            return()
        endif()
    endforeach()


    # if(EXISTS "${resource_file_name}")
    #     file(READ "${resource_file_name}" hex_content HEX)

    #     string(REPEAT "[0-9a-f]" 32 pattern)
    #     string(REGEX REPLACE "(${pattern})" "\\1\n" content "${hex_content}")

    #     string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

    #     string(REGEX REPLACE ", $" "" content "${content}")

    #     set(array_definition "static const std::vector<unsigned char> ${variable_name} =\n{\n${content}\n};")
        
    #     get_filename_component(file_name ${source_file_name} NAME)
    #     set(source "/**\n * @file ${file_name}\n * @brief Auto generated file.\n */\n#include <vector>\n${array_definition}\n")

    #     file(WRITE "${source_file_name}" "${source}")
    # else()
    #     message("ERROR: ${resource_file_name} doesn't exist")
    #     return()
    # endif()

endfunction()

# let's use it as a script
if(EXISTS "${PATH}")
    embed_resource("${PATH}" "${HEADER}" "${GLOBAL}")
endif()
