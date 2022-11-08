#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

#include "allocator.h"

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


    template<typename T>
    class Manager : public PublicSingleton<Manager<T>>
    {
        friend class PublicSingleton<Manager<T>>;
    protected:
        Manager() = default;
        Allocator<std::shared_ptr<T>> m_allocator;

    public:
        Manager(const Manager&)   = delete;
        Manager& operator=(const Manager&) = delete;

        void add(std::shared_ptr<T> t)
        { 
            m_allocator.allocGuid(t);
        }
        std::shared_ptr<T> get(uint32_t t_id)
        {
            return m_allocator.getGuidRelatedElement(t_id);
        }
    };
}