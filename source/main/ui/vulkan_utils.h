#pragma once

#define GLFW_INCLUDE_VULKAN
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <chrono>
#include <optional>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <array>
#include <iostream>
#include <limits>
#include <algorithm>



//#include "window.h"
#include "stb_image.h"
#include "math/math.h"

#include "shader/generated/include/shader_vert.h"
#include "shader/generated/include/shader_frag.h"
#include "shader/generated/include/fillCellVertexInfo_comp.h"
#include "shader/generated/include/physicsCompute_comp.h"
#include "shader/generated/include/resetCellVertexInfo_comp.h"
#include "shader/generated/include/sphereVertexCompute_comp.h"

namespace Sirion {
    struct UniformBufferObject
    {
        alignas(16) Matrix4 model;
        alignas(16) Matrix4 view;
        alignas(16) Matrix4 proj;
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR          capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

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

	class VkUtils
    {
    public:
        static VkShaderModule createShaderModule(VkDevice& device, const std::vector<unsigned char>& shader_code);
    };

	class VkInstanceWrapper {
	private:
#pragma region move
        VkClearColorValue     m_clear_color  = {0.f, 0.f, 0.f, 1.f};
        std::string           m_texture_path = "../assets/images/viking_room.png";
        std::string           m_sphere_path  = "../assets/models/sphere.obj";
        std::string           m_model_path   = "../assets/models/viking_room.obj";
        std::vector<Vertex>   m_raw_verts;
        std::vector<uint32_t> m_raw_indices;
        std::vector<Vertex>   m_sphere_verts;
        std::vector<uint32_t> m_sphere_indices;
        uint32_t              m_num_grid_cells = 1000000;
        uint32_t              m_num_sides = 6;
        uint32_t              m_num_vis        = m_num_sides * m_num_sides * m_num_sides;
        float                 m_delta_t        = 0.0017f;
        uint32_t              m_width          = 1000;
        uint32_t              m_height         = 800;
        float           m_sphere_scale = 0.01f;
        const Matrix4 m_sphere_scale_mat = glm::scale(Matrix4(1), Vector3(m_sphere_scale, m_sphere_scale, m_sphere_scale));
        const int m_max_frames_in_flight = 2;

        // Vertex* raw_verts = new Vertex[N_FOR_VIS];
        // uint32_t* raw_indices = new uint32_t[N_FOR_VIS];

        int* m_cellVertArray = new int[m_num_grid_cells * 6] {0};
        int* m_cellVertCount = new int[m_num_grid_cells] {0};

        std::vector<Vertex>   m_model_verts;
        std::vector<uint32_t> m_model_indices;

        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;
        // Old vertices data
        VkBuffer       m_vertexBuffer1;
        VkDeviceMemory m_vertexBufferMemory1;

        // New vertices data
        VkBuffer       m_vertexBuffer2;
        VkDeviceMemory m_vertexBufferMemory2;

        VkBuffer       m_numVertsBuffer;
        VkDeviceMemory m_numVertsBufferMemory;

        // storge buffer
        VkBuffer       m_cellVertArrayBuffer;
        VkDeviceMemory m_cellVertArrayBufferMemory;
        VkBuffer       m_cellVertCountBuffer;
        VkDeviceMemory m_cellVertCountBufferMemory;

        // sphere vertices storge buffer
        VkBuffer       m_sphereVertsBuffer;
        VkDeviceMemory m_sphereVertsBufferMemory;

        VkBuffer       m_indexBuffer;
        VkDeviceMemory m_indexBufferMemory;

        std::vector<VkBuffer>       m_uniformBuffers;
        std::vector<VkDeviceMemory> m_uniformBuffersMemory;
        std::vector<void*>          m_uniformBuffersMapped;

        VkDescriptorPool                                                m_descriptorPool;
        std::vector<VkDescriptorSet>                                    m_descriptorSets;
        std::vector<VkCommandBuffer>   m_commandBuffers;

        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence>     m_inFlightFences;
        size_t                   m_currentFrame = 0;

        // for compute pipeline
        VkPipelineLayout             m_computePipelineLayout;
        VkPipeline                   m_computePipelinePhysics;
        VkPipeline                   m_computePipelineFillCellVertex;
        VkPipeline                   m_computePipelineResetCellVertex;
        VkPipeline                   m_computePipelineSphereVertex;
        VkDescriptorSetLayout        m_computeDescriptorSetLayout;
        VkDescriptorPool             m_computeDescriptorPool;
        std::vector<VkDescriptorSet> m_computeDescriptorSet;

        bool m_framebufferResized = false;

#pragma endregion
		VkInstance m_instance;
		VkInstanceCreateInfo m_createInfo{};
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		VkDevice m_logicalDevice;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
        VkQueue m_computeQueue;
		VkSurfaceKHR m_surface;

		VkSwapchainKHR m_swapChain;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;

		VkRenderPass m_renderPass;
        VkDescriptorSetLayout m_descriptorSetLayout;
        VkPipelineLayout m_pipelineLayout;
        VkPipeline       m_graphicsPipeline;

        VkImage          m_textureImage;
        VkDeviceMemory   m_textureImageMemory;
        VkImageView      m_textureImageView;
        VkSampler        m_textureSampler;


        VkImage        m_depthImage;
        VkDeviceMemory m_depthImageMemory;
        VkImageView    m_depthImageView;

		VkCommandPool   m_commandPool;
        VkCommandBuffer m_commandBuffer;
        std::vector<VkFramebuffer> m_swapChainFramebuffers;


		// AppInfo setup
		VkApplicationInfo* initAppInfo();

		// Extensions setup
		void initExtensions();
		bool checkExtensions();

		// Validation layers setup
		void initValidationLayers();
		bool checkValidationLayers();

		// Queues
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device);

		// Devices setup
		void setupPhysicalDevice();
		bool checkPhysicalDevice(VkPhysicalDevice& device);
		void setupLogicalDevice();

		// Window surface setup
        void setupWindowSurface(GLFWwindow * window);

		// Swapchain setup
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

        void setupSwapChain(GLFWwindow* window);
        void recreateSwapChain(GLFWwindow* window);

		// ImageView setup
		void setupImageViews();

		// Pipeline setup
        void setupRenderPass();
        void setupGraphicsPipeline();
		void setupFixedFunctions();

		void setupCommandPool();
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling                tiling,
                                     VkFormatFeatureFlags         features);
        VkFormat findDepthFormat();
        void     createImage(uint32_t              width,
                             uint32_t              height,
                             VkFormat              format,
                             VkImageTiling         tiling,
                             VkImageUsageFlags     usage,
                             VkMemoryPropertyFlags properties,
                             VkImage&              image,
                             VkDeviceMemory&       imageMemory);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        void setupDepthResources();
        void setupFramebuffers();
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        VkCommandBuffer beginSingleTimeCommands();
        void            endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void            copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void setupTextureImage();
        void setupTextureImageView();
        void setupTextureSampler();

        // loadModel;
        void createBuffer(VkDeviceSize          size,
                          VkBufferUsageFlags    usage,
                          VkMemoryPropertyFlags properties,
                          VkBuffer&             buffer,
                          VkDeviceMemory&       bufferMemory);
        void copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, const VkDeviceSize& size);
        template<typename DataType>
        void createDataBuffer(VkDeviceSize    bufferSize,
                              VkBuffer&       buffer,
                              VkDeviceMemory& bufferMemory,
                              DataType*       data_to_buffer,
                              VkBufferUsageFlags bufferUsageFlag = (VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
                              VkMemoryPropertyFlags memoryPropertyFlag = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
        void createVertexBuffers();
        void createIndexBuffer();
        void createNumVertsBuffer();

        void createCellVertArrayBuffer();
        void createCellVertCountBuffer();
        void createSphereVertsBuffer();

        void createComputePipeline(const std::vector<unsigned char>& shaderCode, VkPipeline & pipeline);

        void createUniformBuffers();
        void updateUniformBuffer(uint32_t currentImage);
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();

        void cleanupSwapChain();

	public:
		VkInstanceWrapper();

        void init(GLFWwindow * window);
        void idle();
        void drawFrame(GLFWwindow * window);
		~VkInstanceWrapper();

#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    };
} // namespace Sirion