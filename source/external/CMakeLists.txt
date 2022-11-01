cmake_minimum_required (VERSION 3.8)

# Function for building a 3rd-party dependency
function(buildDependency DEPENDENCY_NAME)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_LIBRARY_DIR}/${DEPENDENCY_NAME})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIBRARY_DIR}/${DEPENDENCY_NAME})

    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR})
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_LIBRARY_DIR}/${DEPENDENCY_NAME})
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_LIBRARY_DIR}/${DEPENDENCY_NAME})
    endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
    add_subdirectory(${DEPENDENCY_NAME})
endfunction(buildDependency)

# Build all dependencies
function(buildDependencies)
	foreach(DEPENDENCY ${DEPENDENCIES})
		buildDependency(${DEPENDENCY})
	endforeach(DEPENDENCY)
endfunction(buildDependencies)

# Add include for 3rd-party dependencies
include_directories("glfw/include")
include_directories("glm")

# Build 3rd-party tools
set(DEPENDENCIES
	"glfw"
)

buildDependencies()