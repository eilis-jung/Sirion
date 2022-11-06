#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../core/core.h"
//#include "../ui/window.h"
#include "../ui/vulkan_utils.h"
#include "../core/component.h"

namespace Sirion
{
    class Application: public Component
    {
    public:
        Application() : Component() {

        }
        void run() {
            //m_window.init();
            initVulkan();
            mainLoop();
        }

        void mouseButtonPressLeft(double pos_x, double pos_y)
        {
            m_mouse_left_down = true;
            return;
        }
        void mouseButtonPressRight(double pos_x, double pos_y)
        {
            m_mouse_right_down = true;
            return;
        }
        void mouseButtonReleaseLeft(double pos_x, double pos_y)
        {
            m_mouse_left_down = false;
            return;
        }
        void mouseButtonReleaseRight(double pos_x, double pos_y)
        {
            m_mouse_right_down = false;
            return;
        }

        virtual void updatePrimitives(float delta_x, float delta_y, float delta_z) { return; };

        void mouseMoveCallback(double pos_x, double pos_y) 
        {
            if (m_mouse_left_down)
            {
                float delta_x = static_cast<float>((m_prev_x - pos_x) * m_mouse_sensitivity);
                float delta_y = static_cast<float>((m_prev_y - pos_y) * m_mouse_sensitivity);
                m_prev_x = pos_x;
                m_prev_y  = pos_y;
                updatePrimitives(delta_x, delta_y, 0.0f);
            }
            else if (m_mouse_right_down)
            {
                double delta_z = static_cast<float>((m_prev_y - pos_y) * 0.05);
                m_prev_y = pos_y;
                updatePrimitives(0.0f, 0.0f, delta_z);
            }
        }
    private:
        GLFWwindow*       m_pWindow;
        //Window m_window;
        VkInstanceWrapper m_vkInstance;
        VkInstance        instance;

        // User behavior
        double m_mouse_sensitivity = 0.5;
        bool   m_mouse_left_down  = false;
        bool   m_mouse_right_down = false;
        double m_prev_x      = 0.0;
        double m_prev_y      = 0.0;
        

        void initVulkan() { m_vkInstance.init(m_pWindow); }
        void mainLoop()
        {
            while (!glfwWindowShouldClose(m_pWindow))
            {
                glfwPollEvents();
                m_vkInstance.drawFrame(m_pWindow);
            }
            m_vkInstance.idle();
        }
    };
}