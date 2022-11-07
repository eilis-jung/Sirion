#include "scene.h"

std::weak_ptr<Sirion::Window> Sirion::Scene::getCurrentWindow() const {
    auto curr_application = m_current_application.lock();

	if (!curr_application)
    {
        return std::weak_ptr<Window>();
    }

    return curr_application->getWindow();
}
