#include "vulkan_utils.h"


void Sirion::VkInstanceWrapper::init(GLFWwindow* window)
{
#if defined(__GNUC__)
    // https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#if defined(__linux__)
    char const* vk_layer_path = PICCOLO_XSTR(PICCOLO_VK_LAYER_PATH);
    setenv("VK_LAYER_PATH", vk_layer_path, 1);
#elif defined(__MACH__)
    // https://developer.apple.com/library/archive/documentation/Porting/Conceptual/PortingUnix/compiling/compiling.html
    char const* vk_layer_path    = PICCOLO_XSTR(PICCOLO_VK_LAYER_PATH);
    char const* vk_icd_filenames = PICCOLO_XSTR(PICCOLO_VK_ICD_FILENAMES);
    setenv("VK_LAYER_PATH", vk_layer_path, 1);
    setenv("VK_ICD_FILENAMES", vk_icd_filenames, 1);
#else
#error Unknown Platform
#endif
#elif defined(_MSC_VER)
    // https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

    // For the case when AMD & NVDA GPUs co-exist, and AMD prevents switching to NVDA
    SetEnvironmentVariableA("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
#else
#error Unknown Compiler
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
