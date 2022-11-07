#pragma once

#include "main/serializer/reflection.h"
#include "math/math.h"

namespace Sirion
{
    REFLECTION_TYPE(ApplicationConfig)
    CLASS(ApplicationConfig, Fields)
    {
        REFLECTION_BODY(ApplicationConfig);

    public:
        double m_mouse_sensitivity = 0.5;
        uint32_t m_width = 800;
        uint32_t m_height = 600;
        std::string m_title;

    private:
    };
} // namespace Sirion