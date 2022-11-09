#pragma once

#include <string>
#include "math/math.h"

namespace Sirion
{
    namespace Physics
    {
        const int       N_GRID_CELLS      = 1000000;
        const int       N_SIDE            = 30;
        const int       N_FOR_VIS         = N_SIDE * N_SIDE * N_SIDE;
        const float     DT                = 0.0017f;
        const int       WIDTH             = 1000;
        const int       HEIGHT            = 800;
        const float     BALL_SCALE_FACTOR = 0.01f;
        const Matrix4 BALL_SCALE_MAT =
            glm::scale(Matrix4(1), Vector3(BALL_SCALE_FACTOR, BALL_SCALE_FACTOR, BALL_SCALE_FACTOR));

        const int         MAX_FRAMES_IN_FLIGHT = 2;
        const std::string BALL_PATH            = "../../../asset/models/sphere.obj";
        const std::string TEXTURE_PATH         = "../../../asset/images/viking_room.png";
        const std::string MODEL_PATH           = "../../../asset/models/viking_room.obj";

        bool   leftMouseDown  = false;
        bool   rightMouseDown = false;
        double previousX      = 0.0;
        double previousY      = 0.0;

        float r     = 20.0f;
        float theta = 1.0f;
        float phi   = -0.7f;

        Vector3 eye     = Vector3(5.0f, 10.0f, r);
        Matrix4 viewMat = glm::lookAt(eye, Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 1.0f, 0.0f));

        static void updateOrbit(float deltaX, float deltaY, float deltaZ)
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
            viewMat = glm::inverse(finalTransform);

            eye = Vector3(-viewMat[3][0], -viewMat[3][1], -viewMat[3][2]);
        }

    }
}