# Precompile
add_subdirectory ("meta_parser")
include(precompile.cmake)
add_dependencies(SirionPrecompile "SirionParser")
set(PRECOMPILE_TARGET "SirionPrecompile")
set_target_properties(${PRECOMPILE_TARGET} PROPERTIES FOLDER "Tools")