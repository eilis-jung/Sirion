#pragma once

#include <type_traits>
#include <memory>
#include <iostream>
namespace Sirion
{
    class Component : public std::enable_shared_from_this<Component>
    {
    public:
        static uint32_t m_global_id; // Global ID starts from 1, NOT 0
        Component()
        {
            m_component_id = m_global_id;
            m_global_id++;
            std::cout << "created!" << std::endl;
        }
        Component(Component&& src) noexcept {
            moveFrom(src);
        }
        Component(Component& src) noexcept = delete;
        //Component(const Component& src) { m_component_id = src.getComponentId();
        //    std::cout << "copied!" << std::endl;
        //};
        Component& operator=(const Component& src) = delete;
        Component& operator=(Component&& rhs) noexcept
        {
            if (this == &rhs)
            {
                return *this;
            }
            else
            {
                moveFrom(rhs);
            }
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

        void moveFrom(Component& src)
        {
            test               = src.test;
            m_component_id     = src.m_component_id;
            src.test           = 0;
            src.m_component_id = 0;
        }
    };

    //inline
    //bool operator==(const std::weak_ptr<Component>& lhs, const std::weak_ptr<Component>& rhs)
    //{
    //    return lhs.lock()->getComponentId() == rhs.lock()->getComponentId();
    //}


}



//template<>
//struct std::hash<Sirion::Component>
//{
//    size_t operator()(const Sirion::Component& rhs) const noexcept { 
//        //return 2;
//        return rhs.getComponentId(); 
//    }
//};

template<>
struct std::hash<std::shared_ptr<Sirion::Component>>
{
    size_t operator()(const std::shared_ptr<Sirion::Component>& rhs) const noexcept
    {
        // return 2;
        return rhs->getComponentId();
    }
};