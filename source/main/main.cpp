
#include <filesystem>
#include <fstream>
#include <iostream>


#include "main.h"

int main(int argc, char** argv) {
	Sirion::PointRenderApplication app;
    Sirion::SceneManager::getInstance().addComponent(app);
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}