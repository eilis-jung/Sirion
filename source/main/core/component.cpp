#include "component.h"
#include "main/core/manager/component_manager.h"

std::shared_ptr<Sirion::Component> Sirion::ComponentFactory::create() { return createInstance(); }

std::shared_ptr<Sirion::Component> Sirion::ComponentFactory::createInstance()
{
    m_global_id++;
    auto res = std::make_shared<Component>(m_global_id);
    ComponentManager::getInstance().add(res);
    return res;
}

Sirion::Component& Sirion::Component::operator=(Component&& rhs) noexcept
{
    if (this == &rhs)
        return *this;
    else
        moveFrom(rhs);
}