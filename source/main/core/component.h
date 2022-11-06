#pragma once

#include <type_traits>

namespace Sirion
{
    class Component
    {
    public:
        static uint32_t m_global_id;
        Component()
        {
            m_component_id = m_global_id;
            m_global_id++;
        }
        uint32_t getComponentId() const { 
            return m_component_id;
        }
        bool     operator==(const Sirion::Component& rhs) const { 
            return (m_component_id == rhs.getComponentId());
        };

    private:
        uint32_t m_component_id;
    };


}

template<>
struct std::hash<Sirion::Component>
{
    size_t operator()(const Sirion::Component& rhs) const noexcept { return rhs.getComponentId(); }
};