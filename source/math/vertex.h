#pragma once

#include <algorithm>
#include <array>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "base.h"

namespace Sirion
{
    struct Vertex
    {
        Vector4 position = Vector4(0.f, 0.f, 0.f, 1.f);
        Vector4 velocity = Vector4(0.f, 0.f, 0.f, 1.f);
        Vector4 attr1    = Vector4(0.05f, 0.0125f, -1.f, 1.f);
        ; // radius, mass, isFixed, snowPortion
        Vector4 attr2 = Vector4(-1.f, -1.f, 1.f, 1.f);
        ; // neighborMax, hasBrokenBond, d, (null)
        Vector4 color = Vector4(1.f, 1.f, 1.f, 1.f);

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Vertex);
            bindingDescription.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, position);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, velocity);

            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, attr1);

            attributeDescriptions[3].binding  = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset   = offsetof(Vertex, attr2);

            attributeDescriptions[4].binding  = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[4].offset   = offsetof(Vertex, color);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const
        {
            return position == other.position && velocity == other.velocity && attr1 == other.attr1 &&
                   attr2 == other.attr2 && color == other.color;
        }
    };

}