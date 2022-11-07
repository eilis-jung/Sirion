#pragma once

#include "math/math.h"
#include "main/serializer/reflection.h"

namespace Sirion
{
    REFLECTION_TYPE(RendererConfig)
    CLASS(RendererConfig, Fields)
    {
        REFLECTION_BODY(RendererConfig);

    public:
        #pragma region VisualizationConfigs
        float m_r     = 20.0f;
        float m_theta = 1.0f;
        float m_phi   = -0.7f;
        //Vector3         m_eye     = Vector3(5.0f, 10.0f, m_r);
        //Matrix4 m_viewMat = glm::lookAt(m_eye, Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 1.0f, 0.0f));
        #pragma endregion

        #pragma region SimulationConfigs
        int               m_num_grid_cells;
        int               m_num_sides       = 30;
        float             m_delta_time      = 0.0017f;
        #pragma endregion

        #pragma region ModelConfigs
        std::string m_sphere_obj_path    = "../../../assets/models/sphere.obj";
        std::string m_texture_path = "../../../assets/images/viking_room.png";
        std::string m_model_path   = "../../../assets/models/viking_room.obj";
        float       m_sphere_scale       = 0.01f;
        //Matrix4 m_sphere_scale_matrix = glm::scale(Matrix4(1), Vector3(m_sphere_scale, m_sphere_scale, m_sphere_scale));
        #pragma endregion

        #pragma region RenderingConfigs
        int m_max_frames_in_flight = 2;
        #pragma endregion

    private:

    };
}