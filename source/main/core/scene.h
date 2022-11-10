#pragma once

#include <string>
#include <memory>
#include "main/serializer/reflection.h"
#include "main/core/manager/component_manager.h"
#include "main/application/application.h"

namespace Sirion
{
    class SceneManager;
    class Window;

    REFLECTION_TYPE(SceneConfig)
    CLASS(SceneConfig, Fields)
    {
        REFLECTION_BODY(SceneConfig);

    public:
        double m_test = 0.5;

    private:
    };

    class Scene
    {
    public:
        Scene() { 
            init();
        }

        void init() {
            ApplicationFactory applicationFactory;
            m_current_application = std::static_pointer_cast<Application>(applicationFactory.create());
            //m_current_application = std::make_shared<Application>(3);
            ComponentManager::getInstance().add(m_current_application);
            m_current_application->run();
        }
        std::weak_ptr<Application> getCurrentApplication() const { 
            return m_current_application;
        }

        std::weak_ptr<Window> getCurrentWindow() const {
            return m_current_application->getWindow();
        };
    private:
        std::shared_ptr<Application> m_current_application;
        std::string                m_title;
        SceneConfig                m_scene_config;
    };
}

