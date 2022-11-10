#pragma once

#include <iostream>
#include <memory>
#include <type_traits>
namespace Sirion
{
    class Component : public std::enable_shared_from_this<Component>
    {
    public:
        Component(uint32_t global_id) : m_component_id(global_id) {}

        Component(Component& src) noexcept = delete;

        Component(Component&& src) noexcept { moveFrom(src); }

        virtual ~Component() {}
        Component& operator=(const Component& src) = delete;
        Component& operator                        =(Component&& rhs) noexcept;
        uint32_t   getComponentId() const { return m_component_id; }
        bool       operator==(const Sirion::Component& rhs) const { return (m_component_id == rhs.getComponentId()); };

    private:
        uint32_t m_component_id;
        void     moveFrom(Component& src)
        {
            m_component_id     = src.m_component_id;
            src.m_component_id = 0;
        }
    };

    // For components there has to be a factory
    // Since components are always stored in ComponentManager with a shared_ptr
    // Similarly, every derived class of Component should also have a factory
    class ComponentFactory
    {
    public:
        virtual std::shared_ptr<Component> create();

    protected:
        uint32_t                           m_global_id = 0; // Global ID starts from 1, NOT 0
        virtual std::shared_ptr<Component> createInstance();
    };
} // namespace Sirion

template<>
struct std::hash<std::shared_ptr<Sirion::Component>>
{
    size_t operator()(const std::shared_ptr<Sirion::Component>& rhs) const noexcept { return rhs->getComponentId(); }
};