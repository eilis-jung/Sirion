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

    public:
        std::shared_ptr<Physics>   m_physics;
        std::shared_ptr<Particles> m_particles;
        Application(uint32_t global_id) : Component(global_id)
        {
            m_physics   = std::make_shared<Physics>();
            m_particles = std::make_shared<Particles>();
            m_window    = std::make_shared<Window>(m_application_config.m_width,
                                                m_application_config.m_height,
                                                m_application_config.m_title,
                                                m_physics,
                                                m_particles);
        }

        void init()
        {
            m_particles->init();
            m_physics->updateOrbit(0.0f, 0.0f, 0.0f);
        }

        ~Application() {}

        void run() { m_window->loop(); }

        std::weak_ptr<Window> getWindow() { return std::weak_ptr<Window>(m_window); }
    };

    class ApplicationFactory : public ComponentFactory
    {
    protected:
        virtual std::shared_ptr<Component> createInstance()
        {
            m_global_id++;
            auto res = std::make_shared<Application>(m_global_id);
            ComponentManager::getInstance().add(res);
            return res;
        }
    };
} // namespace Sirion
