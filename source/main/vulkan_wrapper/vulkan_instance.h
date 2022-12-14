#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "particles/particles.h"
#include "shader/generated/all.h"
#include "physics/base.h"
#include "vulkan_utils.h"


namespace Sirion
{
    class VulkanInstance
    {
    public:
        VulkanInstance() {}
        void init(GLFWwindow* pWindow, std::shared_ptr<Particles> particles, std::shared_ptr<Physics> physics);
        void cleanup();
        void drawFrame();
        void idle();
        std::shared_ptr<Physics> getPhysics() { return m_physics; }

    private:
        std::string m_texture_path = "../../../asset/images/viking_room.png";
        GLFWwindow* m_window;
        int         m_max_frames_in_flight = 2;

        std::shared_ptr<Particles>   m_particles;
        std::shared_ptr<Physics>     m_physics;
        vk::UniqueInstance           m_instance;
        vk::SurfaceKHR               m_surface;
        vk::PhysicalDevice           m_physicalDevice;
        vk::UniqueDevice             m_device;
        vk::Queue                    m_graphicsQueue;
        vk::Queue                    m_presentQueue;
        vk::Queue                    m_computeQueue;
        vk::SwapchainKHR             m_swapChain;
        std::vector<vk::Image>       m_swapChainImages;
        vk::Format                   m_swapChainImageFormat;
        vk::Extent2D                 m_swapChainExtent;
        std::vector<vk::ImageView>   m_swapChainImageViews;
        std::vector<vk::Framebuffer> m_swapChainFramebuffers;
        vk::RenderPass               m_renderPass;
        vk::DescriptorSetLayout      m_descriptorSetLayout;
        vk::PipelineLayout           m_pipelineLayout;
        vk::Pipeline                 m_graphicsPipeline;
        vk::CommandPool              m_commandPool;
        vk::Image                    m_textureImage;
        vk::DeviceMemory             m_textureImageMemory;
        vk::ImageView                m_textureImageView;
        vk::Sampler                  m_textureSampler;
        vk::Image                    m_depthImage;
        vk::DeviceMemory             m_depthImageMemory;
        vk::ImageView                m_depthImageView;
        vk::Buffer                   m_vertexBuffer1;
        vk::DeviceMemory             m_vertexBufferMemory1;
        vk::Buffer                   m_vertexBuffer2;
        vk::DeviceMemory             m_vertexBufferMemory2;
        vk::Buffer                   m_numVertsBuffer;
        vk::DeviceMemory             m_numVertsBufferMemory;
        vk::Buffer                   m_cellVertArrayBuffer;
        vk::DeviceMemory             m_cellVertArrayBufferMemory;
        vk::Buffer                   m_cellVertCountBuffer;
        vk::DeviceMemory             m_cellVertCountBufferMemory;

        int* m_cellVertArray;
        int* m_cellVertCount;

        vk::Buffer       m_sphereVertsBuffer;
        vk::DeviceMemory m_sphereVertsBufferMemory;

        vk::Buffer       m_indexBuffer;
        vk::DeviceMemory m_indexBufferMemory;

        std::vector<vk::Buffer>       m_uniformUboBuffers;
        std::vector<vk::DeviceMemory> m_uniformUboBuffersMemory;

        vk::DescriptorPool                                                m_descriptorPool;
        std::vector<vk::DescriptorSet>                                    m_descriptorSets;
        std::vector<vk::CommandBuffer, std::allocator<vk::CommandBuffer>> m_commandBuffers;

        std::vector<vk::Semaphore> m_imageAvailableSemaphores;
        std::vector<vk::Semaphore> m_renderFinishedSemaphores;
        std::vector<vk::Fence>     m_inFlightFences;
        size_t                     m_currentFrame       = 0;
        bool                       m_framebufferResized = false;

        // for compute pipeline
        vk::PipelineLayout             m_computePipelineLayout;
        vk::Pipeline                   m_computePipelinePhysics;
        vk::Pipeline                   m_computePipelineFillCellVertex;
        vk::Pipeline                   m_computePipelineResetCellVertex;
        vk::Pipeline                   m_computePipelineSphereVertex;
        vk::DescriptorSetLayout        m_computeDescriptorSetLayout;
        vk::DescriptorPool             m_computeDescriptorPool;
        std::vector<vk::DescriptorSet> m_computeDescriptorSet;

        bool                                 checkValidationLayerSupport();
        std::vector<const char*>             getRequiredExtensions();
        void                                 createInstance();
        void                                 createSurface();
        bool                                 isDeviceSuitable(const vk::PhysicalDevice& device);
        VulkanUtils::QueueFamilyIndices      findQueueFamilies(vk::PhysicalDevice device);
        bool                                 checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
        VulkanUtils::SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device);
        void                                 pickPhysicalDevice();
        void                                 createLogicalDevice();
        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        vk::PresentModeKHR   chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
        vk::Extent2D         chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
        void                 createSwapChain();
        vk::ImageView
                   createImageView(vk::Image            image,
                                   vk::Format           format,
                                   vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlags(vk::ImageAspectFlagBits::eColor));
        void       createImageViews();
        vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates,
                                       vk::ImageTiling                tiling,
                                       vk::FormatFeatureFlags         features);
        vk::Format findDepthFormat();
        void       createRenderPass();
        void       createDescriptorSetLayout();
        void       createGraphicsPipeline();
        void       createCommandPool();
        uint32_t   findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
        void       createImage(uint32_t                width,
                               uint32_t                height,
                               vk::Format              format,
                               vk::ImageTiling         tiling,
                               vk::ImageUsageFlags     usage,
                               vk::MemoryPropertyFlags properties,
                               vk::Image&              image,
                               vk::DeviceMemory&       imageMemory);
        void       createDepthResources();
        void       createFramebuffers();
        void       createBuffer(vk::DeviceSize          size,
                                vk::BufferUsageFlags    usage,
                                vk::MemoryPropertyFlags properties,
                                vk::Buffer&             buffer,
                                vk::DeviceMemory&       bufferMemory);
        void
        transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        vk::CommandBuffer beginSingleTimeCommands();
        void              endSingleTimeCommands(vk::CommandBuffer& commandBuffer);
        void              copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
        void              createTextureImage();
        void              createTextureImageView();
        void              createTextureSampler();
        void              copyBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, const vk::DeviceSize& size);
        void              createVertexBuffers();
        void              createIndexBuffer();
        void              createNumVertsBuffer();
        void              createCellVertArrayBuffer();
        void              createCellVertCountBuffer();
        void              createSphereVertsBuffer();
        void createComputePipeline(const std::vector<unsigned char>& shaderCode, vk::Pipeline& pipelineIdx);
        void createuniformUboBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();
        void cleanupSwapChain();
        void recreateSwapChain();
        void updateUniformBuffer(uint32_t currentImage);
    };
} // namespace Sirion