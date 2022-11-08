#include "window.h"
namespace Sirion
{
    bool   Sirion::Window::m_mouse_left_down   = false;
    bool   Sirion::Window::m_mouse_right_down  = false;
    double Sirion::Window::m_prev_x            = 0;
    double Sirion::Window::m_prev_y            = 0;
    double Sirion::Window::m_mouse_sensitivity = 0.5;

    void Sirion::Window::mouseDownCallback(GLFWwindow* window, int button, int action, int mods)
    {
        double pos_x, pos_y;
        glfwGetCursorPos(window, &pos_x, &pos_y);

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            if (action == GLFW_PRESS)
                mouseButtonPressLeft(pos_x, pos_y);
            else if (action == GLFW_RELEASE)
                mouseButtonReleaseLeft(pos_x, pos_y);
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)
                if (action == GLFW_PRESS)
                    mouseButtonPressRight(pos_x, pos_y);
                else if (action == GLFW_RELEASE)
                    mouseButtonReleaseRight(pos_x, pos_y);
    }

    void Sirion::Window::mouseButtonPressLeft(double pos_x, double pos_y)
    {
        m_mouse_left_down = true;
        return;
    }
    void Sirion::Window::mouseButtonPressRight(double pos_x, double pos_y)
    {
        m_mouse_right_down = true;
        return;
    }
    void Sirion::Window::mouseButtonReleaseLeft(double pos_x, double pos_y)
    {
        m_mouse_left_down = false;
        return;
    }
    void Sirion::Window::mouseButtonReleaseRight(double pos_x, double pos_y)
    {
        m_mouse_right_down = false;
        return;
    }

    void Sirion::Window::mouseMoveCallback(GLFWwindow* window, double pos_x, double pos_y)
    {
        if (m_mouse_left_down)
        {
            float delta_x = static_cast<float>((m_prev_x - pos_x) * m_mouse_sensitivity);
            float delta_y = static_cast<float>((m_prev_y - pos_y) * m_mouse_sensitivity);
            m_prev_x      = pos_x;
            m_prev_y      = pos_y;
            //updatePrimitives(delta_x, delta_y, 0.0f);
        }
        else if (m_mouse_right_down)
        {
            float delta_z = static_cast<float>((m_prev_y - pos_y) * 0.05f);
            m_prev_y      = pos_y;
            //updatePrimitives(0.0f, 0.0f, delta_z);
        }
    }
}