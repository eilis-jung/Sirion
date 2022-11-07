#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <functional>
#include "../application/application.h"
#include "../core/manager.h"

namespace Sirion
{
    const int WIN_INITIAL_WIDTH  = 800;
    const int WIN_INITIAL_HEIGHT = 600;

    class Window
    {
    public:
        Window() : m_width(WIN_INITIAL_WIDTH), m_height(WIN_INITIAL_HEIGHT), m_title(""), m_pGLFWWindow(nullptr)
        {
            init();
            m_app.init(m_pGLFWWindow);
            SceneManager::getInstance().addComponent(m_app);
            m_app.run();
        }
        bool        init();
        GLFWwindow* getPointer() { return m_pGLFWWindow; }
        ~Window();

    private:
        GLFWwindow * m_pGLFWWindow;
        uint32_t    m_width;
        uint32_t    m_height;
        std::string m_title;
        Application m_app;

        static void mouseDownCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double pos_x, double pos_y);
    };
} // namespace Sirion