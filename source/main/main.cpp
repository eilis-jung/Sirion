
#include <filesystem>
#include <fstream>
#include <iostream>


#include "main.h"


int main(int argc, char** argv) {

#if defined(_MSC_VER)
    // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
    // For the case when AMD & NVDA GPUs co-exist, and AMD prevents switching to NVDA
    SetEnvironmentVariableA("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
#else
#error Unknown Compiler
#endif
    try
    {
        Sirion::VulkanSpheresRenderer myRender;
        myRender.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
	//Sirion::Scene scene;
    return EXIT_SUCCESS;
}