#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <tiny_obj_loader.h>
#include <vector>

#include "math/math.h"

namespace Sirion
{
    namespace PointsGenerator
    {
        void createCube(std::vector<Vertex>&   verts,
                        std::vector<uint32_t>& indices,
                        int&                   idxForWholeVertices,
                        const int              N_SIDE,
                        const Vector3&         OFFSET,
                        const Vector3&         COLOR);

        void createSphere(std::vector<Vertex>&   verts,
                          std::vector<uint32_t>& indices,
                          int&                   idxForWholeVertices,
                          const int              N_SIDE,
                          const Vector3&         OFFSET,
                          const Vector3&         COLOR,
                          const Vector3&         InitialVel = Vector3(0.f));

        bool inTanglecube(const Vector3& p);

        void createTanglecube(std::vector<Vertex>&   verts,
                              std::vector<uint32_t>& indices,
                              int&                   idxForWholeVertices,
                              const int              N_SIDE,
                              const Vector3&         OFFSET,
                              const Vector3&         COLOR);

        bool inHeart(const Vector3& p);

        void createHeart(std::vector<Vertex>&   verts,
                         std::vector<uint32_t>& indices,
                         int&                   idxForWholeVertices,
                         const int              N_SIDE,
                         const Vector3&         OFFSET,
                         const Vector3&         COLOR);

        bool inTorus(const Vector3& p);

        void createTorus(std::vector<Vertex>&   verts,
                         std::vector<uint32_t>& indices,
                         int&                   idxForWholeVertices,
                         const int              N_SIDE,
                         const Vector3&         OFFSET,
                         const Vector3&         COLOR);
    } // namespace PointsGenerator

    class Particles
    {
    public:
        Particles() { init();
        };
        std::vector<Vertex>   m_verts;
        std::vector<uint32_t> m_indices;
        std::vector<Vertex>   m_raw_verts;
        std::vector<uint32_t> m_raw_indices;
        std::vector<Vertex>   m_sphere_verts;
        std::vector<uint32_t> m_sphere_indices;

        std::string m_sphere_obj_path = "../../../asset/models/sphere.obj";
        std::string m_texturePath     = "../../../asset/images/viking_room.png";
        std::string m_modelPath       = "../../../asset/models/viking_room.obj";

        void init()
        {
            loadModel(m_sphere_obj_path);
            int             idxForWholeVertices = 0;
            const Vector3 OFFSET(0.05f, 0.05f, 0.05f);
            PointsGenerator::createHeart(m_raw_verts,
                                              m_raw_indices,
                                              idxForWholeVertices,
                                              45,
                                              Vector3(2.05f, 3.06f, 2.05f),
                                              Vector3(1.f, 0.f, 0.f));

            int sphereIdx = 0;
            for (int i = 0; i < m_raw_verts.size(); i++)
            {
                auto translation = m_raw_verts[i].position;
                translation.w    = 0.f;
                for (int j = 0; j < m_verts.size(); j++)
                {
                    Vertex v = m_verts[j];
                    v.position += translation;
                    m_sphere_verts.push_back(v);
                    m_sphere_indices.push_back(sphereIdx);
                    sphereIdx++;
                }
            }
            std::cout << "Number of raw_verts: " << m_raw_verts.size() << std::endl;
            std::cout << "Number of sphereIdx: " << sphereIdx << std::endl;
        }

    private:
        void loadModel(std::string modelPath)
        {
            tinyobj::attrib_t                attrib;
            std::vector<tinyobj::shape_t>    shapes;
            std::vector<tinyobj::material_t> materials;
            std::string                      warn, err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
            {
                std::cerr << warn + err << std::endl;
                return;
            }

            const float scale = 0.2f;
            for (const auto& shape : shapes)
            {
                for (const auto& index : shape.mesh.indices)
                {
                    Vertex vertex {};

                    Vector3 pos = {attrib.vertices[3 * index.vertex_index + 0],
                                   attrib.vertices[3 * index.vertex_index + 1],
                                   attrib.vertices[3 * index.vertex_index + 2]};

                    vertex.position   = Vector4(pos, 1.f) * scale;
                    vertex.position.w = 1.f;
                    vertex.color      = Vector4(glm::normalize(pos), 1.f);

                    m_verts.push_back(vertex);
                    m_indices.push_back(m_indices.size());
                }
            }

            std::cout << "Model has " << m_verts.size() << "vertices." << std::endl;
        }
    };

} // namespace Sirion
