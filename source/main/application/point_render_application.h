#pragma once

#include "application.h"

namespace Sirion
{
    class PointRenderApplication : public Application
    {
    public:
        void updatePrimitives(float delta_x, float delta_y, float delta_z) { std::cout << "moved!" << std::endl;
        }
    };

    class SphereRenderApplication : public Application
    {
    public:
        void updatePrimitives(float delta_x, float delta_y, float delta_z) {

        }
    };
}