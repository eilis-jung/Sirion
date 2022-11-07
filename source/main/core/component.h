#pragma once

#include <type_traits>
#include <memory>

namespace Sirion
{
    class Component : public std::enable_shared_from_this<Component>
    {
    public:
        static uint32_t m_global_id;
        Component()
        {
            m_component_id = m_global_id;
            m_global_id++;
        }
        std::shared_ptr<Component> getPointer() { 
            return shared_from_this(); 
        }
        uint32_t getComponentId() const { 
            return m_component_id;
        }
        bool     operator==(const Sirion::Component& rhs) const { 
            return (m_component_id == rhs.getComponentId());
        };

    private:
        int      test = 3;
        uint32_t m_component_id;
    };


}

template<>
struct std::hash<Sirion::Component>
{
    size_t operator()(const Sirion::Component& rhs) const noexcept { return rhs.getComponentId(); }
};