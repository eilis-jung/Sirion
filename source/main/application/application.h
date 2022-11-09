#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "main/ui/window.h"
//#include "main/vulkan_wrapper/vulkan_wrapper.h"
#include "main/core/manager/component_manager.h"
#include "application_config.h"

namespace Sirion
{
    class Application: public Component
    {
    protected:
        ApplicationConfig m_application_config;
        std::shared_ptr<Window> m_window;
        //VulkanWrapper m_vkInstance;
        //VkInstance        instance;
    public:
        Application() : Component()
        {
            m_window = std::make_shared<Window>(
                m_application_config.m_width, m_application_config.m_height, m_application_config.m_title);
        }
        ~Application() {
        }
        void run() {
            m_window->loop();
        }

        std::weak_ptr<Window> getWindow() {
            return std::weak_ptr<Window>(m_window);
        }

        virtual void updatePrimitives(float delta_x, float delta_y, float delta_z) { return; };
    };
}

