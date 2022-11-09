#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>

#include "vulkan_utils.h"

namespace Sirion
{
    class VulkanInstance
    {
    public:
        void init() {
            createInstance();
            createSurface();
            pickPhysicalDevice();
        };

    private:
        GLFWwindow*              m_window;
        vk::UniqueInstance       m_instance;
        vk::SurfaceKHR           m_surface;
        vk::PhysicalDevice       m_physicalDevice;
        bool                     checkValidationLayerSupport();
        std::vector<const char*> getRequiredExtensions();
        void                     createInstance();
        void                     createSurface();
        bool                     isDeviceSuitable(const vk::PhysicalDevice& device);
        QueueFamilyIndices       findQueueFamilies(vk::PhysicalDevice device);
        bool                     checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
        SwapChainSupportDetails  querySwapChainSupport(const vk::PhysicalDevice& device);
        void                     pickPhysicalDevice();
        
        


    };
}