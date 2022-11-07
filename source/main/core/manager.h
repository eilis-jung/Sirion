#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

#include "allocator.h"
#include "component.h"

namespace Sirion
{
    template<typename T>
    class PublicSingleton
    {
    protected:
        PublicSingleton() = default;

    public:
        static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value)
        {
            static T instance;
            return instance;
        }
        virtual ~PublicSingleton() noexcept     = default;
        PublicSingleton(const PublicSingleton&) = delete;
        PublicSingleton& operator=(const PublicSingleton&) = delete;
    };


    class SceneManager final : public PublicSingleton<SceneManager>
    {
        friend class PublicSingleton<SceneManager>;

    private:
        SceneAllocator<Component> m_component_allocator;

    protected:
        SceneManager() = default;

    public:
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        void addComponent(Component & component)
        { 
            m_component_allocator.allocGuid(component);
        }
        bool getComponent(uint32_t component_id, Component & comp)
        {
            Component res;
            m_component_allocator.getGuidRelatedElement(component_id, comp);
            //return res.weak_from_this();
            return true;
        }
    };

}