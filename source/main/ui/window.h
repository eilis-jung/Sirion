#pragma once

#define GLFW_INCLUDE_VULKAN
//#include "../application/application.h"
#include "main/core/manager.h"
#include <GLFW/glfw3.h>
#include <functional>

namespace Sirion
{
    class Window
    {
    private:
        GLFWwindow*            m_pGLFWWindow;
        uint32_t    m_width;
        uint32_t    m_height;
        std::string            m_title;
    public:
        Window(uint32_t width, uint32_t height, std::string & title) : m_width(width), m_height(height), m_title(title), m_pGLFWWindow(nullptr)
        {
            init();
            //m_app.init(m_pGLFWWindow);
            //auto ppp = m_app.getPointer();
            //SceneManager::getInstance().addComponent(m_app);
            //m_app.run();
        }
        bool init()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            m_pGLFWWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
            if (m_pGLFWWindow == nullptr)
                return false;

            glfwSetWindowUserPointer(m_pGLFWWindow, this);
            glfwSetMouseButtonCallback(m_pGLFWWindow, Window::mouseDownCallback);
            glfwSetCursorPosCallback(m_pGLFWWindow, Window::mouseMoveCallback);
            // glfwSetFramebufferSizeCallback(m_window, Window::framebufferResizeCallback);

            return true;
        }

        void loop() {
            while (!glfwWindowShouldClose(m_pGLFWWindow))
            {
                glfwPollEvents();
            }
        }

        GLFWwindow* getPointer() { return m_pGLFWWindow; }
        ~Window()
        {
            //m_app.cleanup();
            glfwDestroyWindow(m_pGLFWWindow);
            glfwTerminate();
        }

    private:
        static void mouseDownCallback(GLFWwindow* window, int button, int action, int mods)
        {
            double pos_x, pos_y;
            glfwGetCursorPos(window, &pos_x, &pos_y);

             //The first component in memory is always Application
            //auto app = SceneManager::getInstance().getComponent(1);
            //auto com = app.lock();
            //auto app2 = std::static_pointer_cast<Application>(com);
            //auto pp  = app.get();

            
            //if (button == GLFW_MOUSE_BUTTON_LEFT)
            //    if (action == GLFW_PRESS)
            //        pp->mouseButtonPressLeft(pos_x, pos_y);
            //    else if (action == GLFW_RELEASE)
            //        pp->mouseButtonReleaseLeft(pos_x, pos_y);
            //    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            //        if (action == GLFW_PRESS)
            //            pp->mouseButtonPressRight(pos_x, pos_y);
            //        else if (action == GLFW_RELEASE)
            //            pp->mouseButtonReleaseRight(pos_x, pos_y);
        }

        static void mouseMoveCallback(GLFWwindow* window, double pos_x, double pos_y)
        {
            //auto app = std::static_pointer_cast<Application>(SceneManager::getInstance().getComponent(1));
        }
    };
} // namespace Sirion