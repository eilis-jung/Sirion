#include "vulkan_utils.h"


void Sirion::VkInstanceWrapper::init(GLFWwindow* window)
{
#if defined(_MSC_VER)
    // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

    // For the case when AMD & NVDA GPUs co-exist, and AMD prevents switching to NVDA
    SetEnvironmentVariableA("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
#endif
    m_createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    m_createInfo.pApplicationInfo = initAppInfo();

    initExtensions();
    initValidationLayers();

    if (vkCreateInstance(&m_createInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    setupWindowSurface(window);
    setupPhysicalDevice();
    setupLogicalDevice();
    setupSwapChain(window);
    setupImageViews();
    setupRenderPass();
    createDescriptorSetLayout();
    setupGraphicsPipeline();

    setupCommandPool();
    setupDepthResources();
    setupFramebuffers();
    setupTextureImage();
    setupTextureImageView();
    setupTextureSampler();

    createVertexBuffers();
    createIndexBuffer();
    createNumVertsBuffer();

    createCellVertArrayBuffer();
    createCellVertCountBuffer();
    createSphereVertsBuffer();

    createComputePipeline(PHYSICSCOMPUTE_COMP, m_computePipelinePhysics);
    createComputePipeline(FILLCELLVERTEXINFO_COMP, m_computePipelineFillCellVertex);
    createComputePipeline(RESETCELLVERTEXINFO_COMP, m_computePipelineResetCellVertex);
    createComputePipeline(SPHEREVERTEXCOMPUTE_COMP, m_computePipelineSphereVertex);

    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}
