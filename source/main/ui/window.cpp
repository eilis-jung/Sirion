#include "window.h"
#include "../application/application.h"
bool Sirion::Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_pGLFWWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_pGLFWWindow == nullptr)
        return false;
    glfwSetWindowUserPointer(m_pGLFWWindow, this);
    //glfwSetFramebufferSizeCallback(m_window, Window::framebufferResizeCallback);
    glfwSetMouseButtonCallback(m_pGLFWWindow, Window::mouseDownCallback);
    glfwSetCursorPosCallback(m_pGLFWWindow, Window::mouseMoveCallback);

    return true;
}

Sirion::Window::~Window()
{
    m_app.cleanup();
    glfwDestroyWindow(m_pGLFWWindow);
    glfwTerminate();
}

inline void Sirion::Window::mouseDownCallback(GLFWwindow* window, int button, int action, int mods)
{
    double pos_x, pos_y;
    glfwGetCursorPos(window, &pos_x, &pos_y);

    // The first component in memory is always Application
    Application app;
    SceneManager::getInstance().getComponent(1, app);

    if (button == GLFW_MOUSE_BUTTON_LEFT)
        if (action == GLFW_PRESS)
            app.mouseButtonPressLeft(pos_x, pos_y);
        else if (action == GLFW_RELEASE)
            app.mouseButtonReleaseLeft(pos_x, pos_y);
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            if (action == GLFW_PRESS)
                app.mouseButtonPressRight(pos_x, pos_y);
            else if (action == GLFW_RELEASE)
                app.mouseButtonReleaseRight(pos_x, pos_y);
}

inline void Sirion::Window::mouseMoveCallback(GLFWwindow* window, double pos_x, double pos_y)
{
    Application app;
    SceneManager::getInstance().getComponent(1, app);
    app.mouseMoveCallback(pos_x, pos_y);
}

