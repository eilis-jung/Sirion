#pragma once

#include "main/serializer/reflection.h"
#include "math/math.h"
#include <string>

namespace Sirion
{
    class Physics
    {
    public:
        static uint32_t m_id;
        int        m_internal_id;
        int     m_numGridCells        = 1000000;
        int     m_numSide             = 30;
        int     m_numForVisualization = m_numSide * m_numSide * m_numSide;
        float   m_deltaT              = 0.0017f;
        float   m_sphereScaleFactor   = 0.01f;
        float   r                     = 20.0f;
        float   theta                 = 1.0f;
        float   phi                   = -0.7f;
        Vector3 eye                   = Vector3(5.0f, 10.0f, r);
        Matrix4 m_viewMat             = glm::lookAt(eye, Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 1.0f, 0.0f));

        Physics() { m_internal_id = m_id;
            m_id++;
        }

        const Matrix4 m_sphereScaleMatrix =
            glm::scale(Matrix4(1), Vector3(m_sphereScaleFactor, m_sphereScaleFactor, m_sphereScaleFactor));
        void updateOrbit(float deltaX, float deltaY, float deltaZ)
        {
            theta += deltaX;
            phi += deltaY;
            r = glm::clamp(r - deltaZ, 1.0f, 50.0f);

            float radTheta = glm::radians(theta);
            float radPhi   = glm::radians(phi);

            Matrix4 rotation = glm::rotate(Matrix4(1.0f), radTheta, Vector3(0.0f, 1.0f, 0.0f)) *
                               glm::rotate(Matrix4(1.0f), radPhi, Vector3(1.0f, 0.0f, 0.0f));
            Matrix4 finalTransform = glm::translate(Matrix4(1.0f), Vector3(0.0f)) * rotation *
                                     glm::translate(Matrix4(1.0f), Vector3(0.0f, 1.0f, r));
            m_viewMat = glm::inverse(finalTransform);

            eye = Vector3(-m_viewMat[3][0], -m_viewMat[3][1], -m_viewMat[3][2]);
        }
        Matrix4 getProjectionMatrix(size_t width, size_t height)
        {
            auto res = glm::perspective(glm::radians(45.f), width / (float)height, 0.01f, 30.0f);
            res[1][1] *= -1;
            return res;
        }

    };
    //REFLECTION_TYPE(Physics)
    //CLASS(Physics, Fields)
    //{
    //    REFLECTION_BODY(Physics);

    //};
} // namespace Sirion