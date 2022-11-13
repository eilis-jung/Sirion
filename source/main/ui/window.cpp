#include "window.h"
#include "main/application/application.h"
namespace Sirion
{
    bool   Sirion::Window::m_mouse_left_down   = false;
    bool   Sirion::Window::m_mouse_right_down  = false;
    double Sirion::Window::m_prev_x            = 0;
    double Sirion::Window::m_prev_y            = 0;
    double Sirion::Window::m_mouse_sensitivity = 5;
    bool   Sirion::Window::framebufferResized  = false;

    void Sirion::Window::mouseDownCallback(GLFWwindow* window, int button, int action, int mods)
    {
        double pos_x, pos_y;
        glfwGetCursorPos(window, &pos_x, &pos_y);

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            if (action == GLFW_PRESS)
                mouseButtonPressLeft(window, pos_x, pos_y);
            else if (action == GLFW_RELEASE)
                mouseButtonReleaseLeft(window, pos_x, pos_y);
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)
                if (action == GLFW_PRESS)
                    mouseButtonPressRight(window, pos_x, pos_y);
                else if (action == GLFW_RELEASE)
                    mouseButtonReleaseRight(window, pos_x, pos_y);
    }

    void Sirion::Window::mouseButtonPressLeft(GLFWwindow* window, double pos_x, double pos_y)
    {
        m_mouse_left_down = true;
        glfwGetCursorPos(window, &m_prev_x, &m_prev_y);
        return;
    }
    void Sirion::Window::mouseButtonPressRight(GLFWwindow* window, double pos_x, double pos_y)
    {
        m_mouse_right_down = true;
        return;
    }
    void Sirion::Window::mouseButtonReleaseLeft(GLFWwindow* window, double pos_x, double pos_y)
    {
        m_mouse_left_down = false;
        return;
    }
    void Sirion::Window::mouseButtonReleaseRight(GLFWwindow* window, double pos_x, double pos_y)
    {
        m_mouse_right_down = false;
        glfwGetCursorPos(window, &m_prev_x, &m_prev_y);
        return;
    }

    void Sirion::Window::mouseMoveCallback(GLFWwindow* window, double pos_x, double pos_y)
    {
        auto                         component  = ComponentManager::getInstance().get(1);
        std::shared_ptr<Application> app       = std::static_pointer_cast<Application>(component);
        if (m_mouse_left_down)
        {
            float delta_x = static_cast<float>((m_prev_x - pos_x) * m_mouse_sensitivity);
            float delta_y = static_cast<float>((m_prev_y - pos_y) * m_mouse_sensitivity);

            m_prev_x = pos_x;
            m_prev_y = pos_y;
            
            app->m_physics.updateOrbit(delta_x, delta_y, 0.0f);
        }
        else if (m_mouse_right_down)
        {
            float delta_z = static_cast<float>((m_prev_y - pos_y) * 0.05f);
            m_prev_y      = pos_y;
            app->m_physics.updateOrbit(0.0f, 0.0f, delta_z);
        }
    }
    void Sirion::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        framebufferResized = true;
        return;
    }
}