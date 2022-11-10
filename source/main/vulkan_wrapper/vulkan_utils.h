#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "math/math.h"

namespace Sirion
{
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    namespace VulkanUtils
    {

        const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

        const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> computeFamily;

            bool isComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
            }
        };

        struct SwapChainSupportDetails
        {
            vk::SurfaceCapabilitiesKHR        capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR>   presentModes;
        };

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        vk::VertexInputBindingDescription getBindingDescription();

        std::array<vk::VertexInputAttributeDescription, 5> getAttributeDescriptions();

        vk::UniqueShaderModule createShaderModule(vk::UniqueDevice&                 device,
                                                  const std::vector<unsigned char>& shader_code);
    } // namespace VulkanUtils

} // namespace Sirion
