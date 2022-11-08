#pragma once
#include "manager.h"
#include "main/core/component.h"

namespace Sirion
{
    //typedef Manager<Component> ComponentManager;

    class ComponentManager : public PublicSingleton<ComponentManager>
    {
        friend class PublicSingleton<ComponentManager>;
    private:
        Allocator<std::shared_ptr<Component>> m_allocator;
    protected:
        ComponentManager() = default;
    public:
        ComponentManager(const ComponentManager&) = delete;
        ComponentManager& operator=(const ComponentManager&) = delete;
        void                     add(std::shared_ptr<Component> t) { m_allocator.allocGuid(t); }
        std::shared_ptr<Component> get(uint32_t t_id) { return m_allocator.getGuidRelatedElement(t_id); }
    };
}

