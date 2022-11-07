#pragma once

#include <string>
#include <memory>
#include "main/serializer/reflection.h"

namespace Sirion
{
    class Application;
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
            std::shared_ptr<Application> app = std::make_shared<Application>();
            m_current_application            = app;
        }
        std::weak_ptr<Application> getCurrentApplication() const { 
            return m_current_application;
        }

        std::weak_ptr<Window> getCurrentWindow() const;
    private:
        std::weak_ptr<Application> m_current_application;
        std::string                m_title;
        SceneConfig                m_scene_config;
    };

    

}
#include "main/core/manager.h"
#include "main/application/application.h"