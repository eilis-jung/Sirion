#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "application_config.h"
#include "main/core/manager/component_manager.h"
#include "main/ui/window.h"
#include "main/vulkan_wrapper/vulkan_instance.h"

namespace Sirion
{
    class Application : public Component
    {
    protected:
        ApplicationConfig       m_application_config;
        std::shared_ptr<Window> m_window;

        Particles m_particles;
        Physics   m_physics;

    public:
        Application() : Component()
        {
            auto physics   = std::make_shared<Physics>(m_physics);
            auto particles = std::make_shared<Particles>(m_particles);
            m_window       = std::make_shared<Window>(m_application_config.m_width,
                                                m_application_config.m_height,
                                                m_application_config.m_title,
                                                physics,
                                                particles

            );

        }

        void init()
        {
            m_particles.init();
            m_physics.updateOrbit(0.0f, 0.0f, 0.0f);
            // m_vulkanInstance = std::make_shared<VulkanInstance>(m_window->getPointer());
            // m_window->assignVulkan(std::weak_ptr<VulkanInstance>(m_vulkanInstance));
        }

        ~Application() {}

        void run() { m_window->loop(); }

        std::weak_ptr<Window> getWindow() { return std::weak_ptr<Window>(m_window); }

        virtual void updatePrimitives(float delta_x, float delta_y, float delta_z) { return; };
    };
} // namespace Sirion
