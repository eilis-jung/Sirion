#pragma once

#include "application.h"

namespace Sirion
{
    class PointRenderApplication : public Application
    {
    public:
        void updatePrimitives(float delta_x, float delta_y, float delta_z) { std::cout << "moved!" << std::endl;
        }
        void mouseButtonPressLeft(double pos_x, double pos_y) override
        {
            m_mouse_left_down = true;
            std::cout << "PointRenderApplication clicked" << std::endl;
            return;
        }
        void mouseButtonPressRight(double pos_x, double pos_y) override
        {
            m_mouse_left_down = true;
            std::cout << "PointRenderApplication clicked" << std::endl;
            return;
        }
        void mouseButtonReleaseLeft(double pos_x, double pos_y) override
        {
            m_mouse_left_down = true;
            std::cout << "PointRenderApplication clicked" << std::endl;
            return;
        }
        void mouseButtonReleaseRight(double pos_x, double pos_y) override
        {
            m_mouse_left_down = true;
            std::cout << "PointRenderApplication clicked" << std::endl;
            return;
        }
    };

    class SphereRenderApplication : public Application
    {
    public:
        void updatePrimitives(float delta_x, float delta_y, float delta_z) {

        }
    };
}