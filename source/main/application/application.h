#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "../ui/window.h"
//#include "../core/core.h"
#include "../ui/vulkan_wrapper.h"
#include "../core/component.h"
#include "../core/scene.h"

#include "application_config.h"

namespace Sirion
{
    //class Window;

    class Application: public Component
    {
    protected:
        ApplicationConfig m_application_config;
        std::weak_ptr<Window> m_window;
        VulkanWrapper m_vkInstance;
        VkInstance        instance;

        
        bool   m_mouse_left_down   = false;
        bool   m_mouse_right_down  = false;
        double m_prev_x            = 0.0;
        double m_prev_y            = 0.0;

    public:
        int h = 30;
        Application() : Component()
        {
            std::shared_ptr<Window> window = std::make_shared<Window>(
                m_application_config.m_width, m_application_config.m_height, m_application_config.m_title);
            m_window                       = window;
            SceneManager::getInstance().addComponent(*this);
            run();
        }
        std::weak_ptr<Window> getWindow() { 
            return m_window;
        }

        void run() {
            Component aaa;
            SceneManager::getInstance().getComponent(1, aaa);
             //auto com = app.lock();
             //auto app2 = std::static_pointer_cast<Application>(com);
             //auto pp  = app.get();
            auto window = m_window.lock();
            //std::cout << Component::m_global_id << std::endl;
            window->loop();
        }

        virtual void mouseButtonPressLeft(double pos_x, double pos_y)
        {
            m_mouse_left_down = true;
            std::cout << "application clicked" << std::endl;
            return;
        }
        virtual void mouseButtonPressRight(double pos_x, double pos_y)
        {
            m_mouse_right_down = true;
            return;
        }
        virtual void mouseButtonReleaseLeft(double pos_x, double pos_y)
        {
            m_mouse_left_down = false;
            return;
        }
        virtual void mouseButtonReleaseRight(double pos_x, double pos_y)
        {
            m_mouse_right_down = false;
            return;
        }

        virtual void updatePrimitives(float delta_x, float delta_y, float delta_z) { return; };

        void mouseMoveCallback(double pos_x, double pos_y) 
        {
            if (m_mouse_left_down)
            {
                float delta_x = static_cast<float>((m_prev_x - pos_x) * m_application_config.m_mouse_sensitivity);
                float delta_y = static_cast<float>((m_prev_y - pos_y) * m_application_config.m_mouse_sensitivity);
                m_prev_x = pos_x;
                m_prev_y  = pos_y;
                updatePrimitives(delta_x, delta_y, 0.0f);
            }
            else if (m_mouse_right_down)
            {
                float delta_z = static_cast<float>((m_prev_y - pos_y) * 0.05f);
                m_prev_y = pos_y;
                updatePrimitives(0.0f, 0.0f, delta_z);
            }
        }
    };
}

