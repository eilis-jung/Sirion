#include "window.h"



bool Sirion::Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (m_window == nullptr)
        return false;
    glfwSetWindowUserPointer(m_window, this);
    //glfwSetFramebufferSizeCallback(m_window, Window::framebufferResizeCallback);
    glfwSetMouseButtonCallback(m_window, Window::mouseDownCallback);
    glfwSetCursorPosCallback(m_window, Window::mouseMoveCallback);

    return true;
}

Sirion::Window::~Window()
{
    glfwDestroyWindow(m_window);
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

