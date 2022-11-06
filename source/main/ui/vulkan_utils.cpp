#include "vulkan_utils.h"

Sirion::VkInstanceWrapper::VkInstanceWrapper() {}


VkApplicationInfo* Sirion::VkInstanceWrapper::initAppInfo()
{
    VkApplicationInfo* appInfo = new VkApplicationInfo{};
    appInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo->pApplicationName = "Hello Triangle";
    appInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo->pEngineName = "No Engine";
    appInfo->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo->apiVersion = VK_API_VERSION_1_0;
    return appInfo;
}


void Sirion::VkInstanceWrapper::initExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);


    if (!checkExtensions()) {
        throw std::runtime_error("Extensions requested, but not available!");
    }

    m_createInfo.enabledExtensionCount = glfwExtensionCount;
    m_createInfo.ppEnabledExtensionNames = glfwExtensions;

}


bool Sirion::VkInstanceWrapper::checkExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return true;
}


void Sirion::VkInstanceWrapper::initValidationLayers()
{
#ifdef NDEBUG
    bool enableValidationLayers = false;
#else
    bool enableValidationLayers = true;
#endif

    const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
    };

    if (enableValidationLayers && !checkValidationLayers()) {
        std::cout << "validation layers requested, but not available!" << std::endl;
        enableValidationLayers = false;
    }

    if (enableValidationLayers) {
        m_createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());

        // Put validationLayers names into heap
        char** layerNames = new char* [validationLayers.size()];
        for (int i = 0; i < validationLayers.size(); i++) {
            layerNames[i] = new char[strlen(validationLayers.data()[i]) + 1];
            strcpy(layerNames[i], validationLayers.data()[i]);
            const_cast<const char*>(layerNames[i]);
        }
        const_cast<const char* const*>(layerNames);
        m_createInfo.ppEnabledLayerNames = layerNames;
        // =====================================
    }
    else {
        m_createInfo.enabledLayerCount = 0;
    }

}


bool Sirion::VkInstanceWrapper::checkValidationLayers()
{
    const std::vector<std::string> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const std::string& layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (layerName == layerProperties.layerName) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}


void Sirion::VkInstanceWrapper::setupPhysicalDevice()
{

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (auto& device : devices) {
        if (checkPhysicalDevice(device)) {
            m_physicalDevice = device;
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}


bool Sirion::VkInstanceWrapper::checkPhysicalDevice(VkPhysicalDevice& device)
{
    return true;
}


void Sirion::VkInstanceWrapper::setupLogicalDevice()
{

    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // Enable swapchain
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
}

void Sirion::VkInstanceWrapper::setupWindowSurface(GLFWwindow * window)
{
    if (glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

Sirion::SwapChainSupportDetails Sirion::VkInstanceWrapper::querySwapChainSupport(VkPhysicalDevice& device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}


VkSurfaceFormatKHR Sirion::VkInstanceWrapper::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}


VkPresentModeKHR Sirion::VkInstanceWrapper::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Sirion::VkInstanceWrapper::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


void Sirion::VkInstanceWrapper::setupSwapChain(GLFWwindow* window)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo {};
    swapChainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface          = m_surface;
    swapChainCreateInfo.minImageCount    = imageCount;
    swapChainCreateInfo.imageFormat      = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace  = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent      = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    // TODO: change to VK_IMAGE_USAGE_TRANSFER_DST_BIT afterwards for processing
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices              = findQueueFamilies(m_physicalDevice);
    uint32_t           queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.queueFamilyIndexCount = 0;       // Optional
        swapChainCreateInfo.pQueueFamilyIndices   = nullptr; // Optional
    }

    swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform; // No pre-transformation

    swapChainCreateInfo.compositeAlpha =
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Ignore alpha channel when blending with other window
    swapChainCreateInfo.presentMode  = presentMode;
    swapChainCreateInfo.clipped      = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_logicalDevice, &swapChainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, m_swapChainImages.data());

    // Store info in member variables for reference in future
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent      = extent;
}

void Sirion::VkInstanceWrapper::recreateSwapChain(GLFWwindow* window)
{
    int width = 0, height = 0;
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    idle();

    cleanupSwapChain();

    setupSwapChain(window);
    setupImageViews();
    setupRenderPass();
    setupGraphicsPipeline();
    setupDepthResources();
    setupFramebuffers();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();



}



void Sirion::VkInstanceWrapper::setupImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_swapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_swapChainImageFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // For mipmapping
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        // For layers in, e.g. stereographic 3D
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;  // Only one layer for now

        if (vkCreateImageView(m_logicalDevice, &imageViewCreateInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}


void Sirion::VkInstanceWrapper::setupRenderPass() {
    VkAttachmentDescription colorAttachment {};
    colorAttachment.format         = m_swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;

    if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void Sirion::VkInstanceWrapper::setupGraphicsPipeline() {
    VkShaderModule vertShaderModule = VkUtils::createShaderModule(m_logicalDevice, SHADER_VERT);
    VkShaderModule fragShaderModule = VkUtils::createShaderModule(m_logicalDevice, SHADER_FRAG);


    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

#pragma region TODO
    auto bindingDescription    = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
#pragma endregion

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

#pragma region TODO
    VkViewport viewport = {};
    viewport.x            = 0.0f;
    viewport.y            = 0.0f;
    viewport.width        = (float)m_swapChainExtent.width;
    viewport.height       = (float)m_swapChainExtent.height;
    viewport.minDepth     = 0.0f;
    viewport.maxDepth     = 1.0f;

    VkRect2D scissor = {};
    scissor.offset     = VkOffset2D {0, 0};
    scissor.extent     = m_swapChainExtent;
#pragma endregion

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pViewports    = &viewport;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable  = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

#pragma region TODO
    VkPipelineDepthStencilStateCreateInfo depthStencil {};
    // The depthTestEnable field specifies if the depth of new fragments should be compared to the depth buffer to see
    // if they should be discarded.
    depthStencil.depthTestEnable  = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    // Specify the comparison that is performed to keep or discard fragments.
    depthStencil.depthCompareOp        = VkCompareOp::VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds        = 0.0f; // Optional
    depthStencil.maxDepthBounds        = 1.0f; // Optional
    depthStencil.stencilTestEnable     = VK_FALSE;
    // depthStencil.front = {}; // Optional
    // depthStencil.back = {}; // Optional
#pragma endregion

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState>      dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 1; // TODO
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pSetLayouts            = &m_descriptorSetLayout;

    if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = m_pipelineLayout;
    pipelineInfo.renderPass          = m_renderPass;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_logicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_logicalDevice, vertShaderModule, nullptr);
}


void Sirion::VkInstanceWrapper::setupFixedFunctions()
{
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // pointer to vertex binding, i.e. spacing between data and whether the data is per-vertex or per-instance
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // pointer to vertex attribute desc, i.e. type of the attributes passed to the vertex shader, which binding to load them from and at which offset

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewports and scissors
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChainExtent.width;
    viewport.height = (float)m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapChainExtent;

    std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;



}

void Sirion::VkInstanceWrapper::setupCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

VkFormat Sirion::VkInstanceWrapper::findSupportedFormat(const std::vector<VkFormat>& candidates,
                                                        VkImageTiling                tiling,
                                                        VkFormatFeatureFlags         features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat Sirion::VkInstanceWrapper::findDepthFormat() { 
            return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

}

void Sirion::VkInstanceWrapper::createImage(uint32_t              width,
                 uint32_t              height,
                 VkFormat              format,
                 VkImageTiling         tiling,
                 VkImageUsageFlags     usage,
                 VkMemoryPropertyFlags properties,
                 VkImage&              image,
                 VkDeviceMemory&       imageMemory)
{
    VkImageCreateInfo imageInfo {};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_logicalDevice, image, imageMemory, 0);
}

uint32_t Sirion::VkInstanceWrapper::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

VkImageView Sirion::VkInstanceWrapper::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo {};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = image;
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = format;
    viewInfo.subresourceRange.aspectMask     = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView;
    if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void Sirion::VkInstanceWrapper::setupDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    VkImageCreateInfo depthImgInfo {};
    depthImgInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImgInfo.imageType  = VK_IMAGE_TYPE_2D;
    depthImgInfo.format        = depthFormat;
    depthImgInfo.extent.width  = m_swapChainExtent.width;
    depthImgInfo.extent.height = m_swapChainExtent.height;
    depthImgInfo.extent.depth  = 1;
    depthImgInfo.mipLevels     = 1;
    depthImgInfo.arrayLayers   = 1;
    depthImgInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    depthImgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImgInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImgInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    depthImgInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

    createImage(m_swapChainExtent.width,
                m_swapChainExtent.height,
                depthFormat,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_depthImage,
                m_depthImageMemory);
    m_depthImageView = createImageView(m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void Sirion::VkInstanceWrapper::setupFramebuffers() {
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {m_swapChainImageViews[i], m_depthImageView};

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = m_renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = m_swapChainExtent.width;
        framebufferInfo.height          = m_swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void Sirion::VkInstanceWrapper::transitionImageLayout(VkImage       image,
                                                      VkFormat      format,
                                                      VkImageLayout oldLayout,
                                                      VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier {};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    endSingleTimeCommands(commandBuffer);


}

VkCommandBuffer Sirion::VkInstanceWrapper::beginSingleTimeCommands() 
{ 
            VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Sirion::VkInstanceWrapper::endSingleTimeCommands(VkCommandBuffer commandBuffer) 
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

void Sirion::VkInstanceWrapper::createBuffer(VkDeviceSize          size,
                                             VkBufferUsageFlags    usage,
                                             VkMemoryPropertyFlags properties,
                                             VkBuffer&             buffer,
                                             VkDeviceMemory&       bufferMemory)
{
    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size        = size;
    bufferInfo.usage       = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_logicalDevice, buffer, bufferMemory, 0);
}

void Sirion::VkInstanceWrapper::copyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, const VkDeviceSize& size) {
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &commandBuffer);
}

void Sirion::VkInstanceWrapper::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region {};
    region.bufferOffset                    = 0;
    region.bufferRowLength                 = 0;
    region.bufferImageHeight               = 0;
    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel       = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;
    region.imageOffset                     = {0, 0, 0};
    region.imageExtent                     = {width, height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);

}

void Sirion::VkInstanceWrapper::setupTextureImage() {
    int          texWidth, texHeight, texChannels;
    stbi_uc*     pixels    = stbi_load(m_texture_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }
    else
    {
        std::cout << "Texture image loaded!" << std::endl;
    }

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* data;
    vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

    stbi_image_free(pixels);


    // Create texture image
    createImage(texWidth,
                texHeight,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_textureImage,
                m_textureImageMemory);

    transitionImageLayout(
        m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    
    // Prepare texture for shader
    transitionImageLayout(m_textureImage,
                          VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);
}

void Sirion::VkInstanceWrapper::setupTextureImageView() {
    m_textureImageView = createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Sirion::VkInstanceWrapper::setupTextureSampler() {
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo {};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_LINEAR;
    samplerInfo.minFilter               = VK_FILTER_LINEAR;
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;

    // Return black when sampling beyond the image with clamp to border addressing mode.
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    // The texels are addressed using the [0, 1) range on all axes.
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;

    // Mipmapping
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 0.0f;

    if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

template<typename DataType>
void Sirion::VkInstanceWrapper::createDataBuffer(VkDeviceSize    bufferSize,
                                                 VkBuffer&       buffer,
                                                 VkDeviceMemory& bufferMemory,
                                                 DataType* data_to_buffer,
                                                 VkBufferUsageFlags bufferUsageFlag,
                                                 VkMemoryPropertyFlags memoryPropertyFlag)
{
#pragma region move
    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer,
                 stagingBufferMemory);

    void* data;
    vkMapMemory(m_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, data_to_buffer, (size_t)bufferSize);
    vkUnmapMemory(m_logicalDevice, stagingBufferMemory);

    createBuffer(bufferSize, bufferUsageFlag, memoryPropertyFlag,
                 buffer,
                 bufferMemory);

    copyBuffer(stagingBuffer, buffer, bufferSize);

    vkDestroyBuffer(m_logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, stagingBufferMemory, nullptr);
#pragma endregion
}

void Sirion::VkInstanceWrapper::createVertexBuffers() {
#pragma region move
    createDataBuffer<Vertex>(
        static_cast<uint32_t>(m_raw_verts.size() * sizeof(Vertex)),
        m_vertexBuffer1,
        m_vertexBufferMemory1,
        m_raw_verts.data(),
        (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        );
    createDataBuffer<Vertex>(
        static_cast<uint32_t>(m_raw_verts.size() * sizeof(Vertex)),
        m_vertexBuffer2,
        m_vertexBufferMemory2,
        m_raw_verts.data(),
        (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
#pragma endregion
}

void Sirion::VkInstanceWrapper::createIndexBuffer() {
#pragma region move
    createDataBuffer<uint32_t>(m_sphere_indices.size() * sizeof(m_sphere_indices[0]),
                     m_indexBuffer,
                     m_indexBufferMemory,
                     m_sphere_indices.data(),
                     (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );
#pragma endregion
}

void Sirion::VkInstanceWrapper::createNumVertsBuffer() {
#pragma region move
    const int num_verts = m_raw_verts.size();
    createDataBuffer<const int>(
        sizeof(int), 
        m_numVertsBuffer, 
        m_numVertsBufferMemory, 
        &num_verts,
        (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
#pragma endregion
}

void Sirion::VkInstanceWrapper::createCellVertArrayBuffer() {
#pragma region move
    createDataBuffer<int*>(static_cast<uint32_t>(sizeof(int) * m_num_grid_cells * 6),
                           m_cellVertArrayBuffer,
                           m_cellVertArrayBufferMemory,
                           &m_cellVertArray,
                           (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
#pragma endregion
}

void Sirion::VkInstanceWrapper::createCellVertCountBuffer() {
#pragma region move
    createDataBuffer<int*>(
        static_cast<uint32_t>(sizeof(int) * m_num_grid_cells),
        m_cellVertCountBuffer,
        m_cellVertCountBufferMemory,
        &m_cellVertCount,
        (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
#pragma endregion
}

void Sirion::VkInstanceWrapper::createSphereVertsBuffer() {
#pragma region move
    createDataBuffer<Vertex>(
        static_cast<uint32_t>(sizeof(Vertex) * m_sphere_verts.size()),
        m_sphereVertsBuffer,
        m_sphereVertsBufferMemory,
        m_sphere_verts.data(),
        (VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
#pragma endregion
}

void Sirion::VkInstanceWrapper::createComputePipeline(const std::vector<unsigned char>& shaderCode, VkPipeline& pipeline)
{
    VkShaderModule computeShaderModule = VkUtils::createShaderModule(m_logicalDevice, shaderCode);


    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {};
    computeShaderStageInfo.flags                           = VkPipelineShaderStageCreateFlags();
    computeShaderStageInfo.stage                           = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module                          = computeShaderModule;
    computeShaderStageInfo.pName                           = "main";

    VkDescriptorSetLayoutBinding computeLayoutBindingVertices1 {};
    computeLayoutBindingVertices1.binding            = 0;
    computeLayoutBindingVertices1.descriptorCount    = 1;
    computeLayoutBindingVertices1.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingVertices1.pImmutableSamplers = nullptr;
    computeLayoutBindingVertices1.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding computeLayoutBindingVertices2 {};
    computeLayoutBindingVertices2.binding            = 1;
    computeLayoutBindingVertices2.descriptorCount    = 1;
    computeLayoutBindingVertices2.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingVertices2.pImmutableSamplers = nullptr;
    computeLayoutBindingVertices2.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding computeLayoutBindingCellVertexArray {};
    computeLayoutBindingCellVertexArray.binding            = 2;
    computeLayoutBindingCellVertexArray.descriptorCount    = 1;
    computeLayoutBindingCellVertexArray.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingCellVertexArray.pImmutableSamplers = nullptr;
    computeLayoutBindingCellVertexArray.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding computeLayoutBindingCellVertexCount {};
    computeLayoutBindingCellVertexCount.binding            = 3;
    computeLayoutBindingCellVertexCount.descriptorCount    = 1;
    computeLayoutBindingCellVertexCount.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingCellVertexCount.pImmutableSamplers = nullptr;
    computeLayoutBindingCellVertexCount.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding computeLayoutBindingNumVerts {};
    computeLayoutBindingNumVerts.binding            = 4;
    computeLayoutBindingNumVerts.descriptorCount    = 1;
    computeLayoutBindingNumVerts.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingNumVerts.pImmutableSamplers = nullptr;
    computeLayoutBindingNumVerts.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding computeLayoutBindingSphereVerts {};
    computeLayoutBindingSphereVerts.binding            = 5;
    computeLayoutBindingSphereVerts.descriptorCount    = 1;
    computeLayoutBindingSphereVerts.descriptorType     = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    computeLayoutBindingSphereVerts.pImmutableSamplers = nullptr;
    computeLayoutBindingSphereVerts.stageFlags         = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = {computeLayoutBindingVertices1,
                                                          computeLayoutBindingVertices2,
                                                          computeLayoutBindingCellVertexArray,
                                                          computeLayoutBindingCellVertexCount,
                                                          computeLayoutBindingNumVerts,
                                                          computeLayoutBindingSphereVerts};

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    // descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.flags        = VkDescriptorSetLayoutCreateFlags();
    descriptorSetLayoutCreateInfo.pNext        = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings    = bindings.data();

    if (vkCreateDescriptorSetLayout(
        m_logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_computeDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create computeDescriptorSetLayout!");
    }

    std::array<VkDescriptorPoolSize, 1> poolSizes {};
    poolSizes[0].type            = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 10;

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.flags         = VkDescriptorPoolCreateFlags();
    descriptorPoolInfo.pNext         = nullptr;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes    = poolSizes.data();
    descriptorPoolInfo.maxSets       = 1;

    if (vkCreateDescriptorPool(m_logicalDevice, &descriptorPoolInfo, nullptr, &m_computeDescriptorPool) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create computeDescriptorPool!");
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.descriptorPool     = m_computeDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts        = &m_computeDescriptorSetLayout;

    if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, m_computeDescriptorSet.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate computeDescriptorSet!");
    }

    // Set descriptor set for the old vertices
    VkDescriptorBufferInfo computeBufferInfoVertices1 = {};
    computeBufferInfoVertices1.buffer                 = m_vertexBuffer1;
    computeBufferInfoVertices1.offset                 = 0;
    computeBufferInfoVertices1.range                  = static_cast<uint32_t>(m_raw_verts.size() * sizeof(Vertex));

    VkWriteDescriptorSet writeComputeInfoVertices1 = {};
    writeComputeInfoVertices1.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoVertices1.dstBinding           = 0;
    writeComputeInfoVertices1.descriptorCount      = 1;
    writeComputeInfoVertices1.dstArrayElement      = 0;
    writeComputeInfoVertices1.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoVertices1.pBufferInfo          = &computeBufferInfoVertices1;

    // Set descriptor set for the new vertices
    VkDescriptorBufferInfo computeBufferInfoVertices2 = {};
    computeBufferInfoVertices2.buffer                 = m_vertexBuffer2;
    computeBufferInfoVertices2.offset                 = 0;
    computeBufferInfoVertices2.range                  = static_cast<uint32_t>(m_raw_verts.size() * sizeof(Vertex));

    VkWriteDescriptorSet writeComputeInfoVertices2 = {};
    writeComputeInfoVertices2.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoVertices2.dstBinding           = 1;
    writeComputeInfoVertices2.descriptorCount      = 1;
    writeComputeInfoVertices2.dstArrayElement      = 0;
    writeComputeInfoVertices2.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoVertices2.pBufferInfo          = &computeBufferInfoVertices2;

    // Set descriptor set for the cell vertex array
    VkDescriptorBufferInfo computeBufferInfoCellVertexArray = {};
    computeBufferInfoCellVertexArray.buffer                 = m_cellVertArrayBuffer;
    computeBufferInfoCellVertexArray.offset                 = 0;
    computeBufferInfoCellVertexArray.range                  = static_cast<uint32_t>(m_num_grid_cells * 6 * sizeof(int));

    VkWriteDescriptorSet writeComputeInfoCellVertexArray = {};
    writeComputeInfoCellVertexArray.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoCellVertexArray.dstBinding           = 2;
    writeComputeInfoCellVertexArray.descriptorCount      = 1;
    writeComputeInfoCellVertexArray.dstArrayElement      = 0;
    writeComputeInfoCellVertexArray.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoCellVertexArray.pBufferInfo          = &computeBufferInfoCellVertexArray;

    // Set descriptor set for the cell vertex count
    VkDescriptorBufferInfo computeBufferInfoCellVertexCount = {};
    computeBufferInfoCellVertexCount.buffer                 = m_cellVertCountBuffer;
    computeBufferInfoCellVertexCount.offset                 = 0;
    computeBufferInfoCellVertexCount.range                  = static_cast<uint32_t>(m_num_grid_cells * sizeof(int));

    VkWriteDescriptorSet writeComputeInfoCellVertexCount = {};
    writeComputeInfoCellVertexCount.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoCellVertexCount.dstBinding           = 3;
    writeComputeInfoCellVertexCount.descriptorCount      = 1;
    writeComputeInfoCellVertexCount.dstArrayElement      = 0;
    writeComputeInfoCellVertexCount.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoCellVertexCount.pBufferInfo          = &computeBufferInfoCellVertexCount;

    // Set descriptor set for the buffer representing the number of vertices
    VkDescriptorBufferInfo computeBufferInfoNumVerts = {};
    computeBufferInfoNumVerts.buffer                 = m_numVertsBuffer;
    computeBufferInfoNumVerts.offset                 = 0;
    computeBufferInfoNumVerts.range                  = static_cast<uint32_t>(sizeof(int));

    VkWriteDescriptorSet writeComputeInfoNumVerts = {};
    writeComputeInfoNumVerts.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoNumVerts.dstBinding           = 4;
    writeComputeInfoNumVerts.descriptorCount      = 1;
    writeComputeInfoNumVerts.dstArrayElement      = 0;
    writeComputeInfoNumVerts.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoNumVerts.pBufferInfo          = &computeBufferInfoNumVerts;

    // Set descriptor set for the buffer representing the sphere vertices
    VkDescriptorBufferInfo computeBufferInfoSphereVerts = {};
    computeBufferInfoSphereVerts.buffer                 = m_sphereVertsBuffer;
    computeBufferInfoSphereVerts.offset                 = 0;
    computeBufferInfoSphereVerts.range                  = static_cast<uint32_t>(sizeof(Vertex) * m_sphere_verts.size());

    VkWriteDescriptorSet writeComputeInfoSphereVerts = {};
    writeComputeInfoSphereVerts.dstSet               = m_computeDescriptorSet[0];
    writeComputeInfoSphereVerts.dstBinding           = 5;
    writeComputeInfoSphereVerts.descriptorCount      = 1;
    writeComputeInfoSphereVerts.dstArrayElement      = 0;
    writeComputeInfoSphereVerts.descriptorType       = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeComputeInfoSphereVerts.pBufferInfo          = &computeBufferInfoSphereVerts;

    std::array<VkWriteDescriptorSet, 6> writeDescriptorSets = {writeComputeInfoVertices1,
                                                               writeComputeInfoVertices2,
                                                               writeComputeInfoCellVertexArray,
                                                               writeComputeInfoCellVertexCount,
                                                               writeComputeInfoNumVerts,
                                                               writeComputeInfoSphereVerts};

    vkUpdateDescriptorSets(
        m_logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
    std::array<VkDescriptorSetLayout, 1> descriptorSetLayouts = {m_computeDescriptorSetLayout};

    VkPipelineLayoutCreateInfo computePipelineLayoutInfo = {};
    computePipelineLayoutInfo.flags                      = VkPipelineLayoutCreateFlags();
    computePipelineLayoutInfo.setLayoutCount             = static_cast<uint32_t>(descriptorSetLayouts.size());
    computePipelineLayoutInfo.pSetLayouts                = descriptorSetLayouts.data();
    computePipelineLayoutInfo.pushConstantRangeCount     = 0;
    computePipelineLayoutInfo.pPushConstantRanges        = 0;


    if (vkCreatePipelineLayout(m_logicalDevice, &computePipelineLayoutInfo, nullptr, &m_computePipelineLayout) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo computePipelineInfo = {};
    computePipelineInfo.flags                       = VkPipelineCreateFlags();
    computePipelineInfo.stage                       = computeShaderStageInfo;
    computePipelineInfo.layout                      = m_computePipelineLayout;

    if (vkCreateComputePipelines(m_logicalDevice, NULL, 0, &computePipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create compute pipeline!");
    }
}

void Sirion::VkInstanceWrapper::createUniformBuffers() {

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    #pragma region move
    m_uniformBuffers.resize(m_swapChainImages.size());
    m_uniformBuffersMemory.resize(m_swapChainImages.size());
    m_uniformBuffersMapped.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        createBuffer(bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     m_uniformBuffers[i],
                     m_uniformBuffersMemory[i]);

        vkMapMemory(m_logicalDevice, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
    }
    #pragma endregion
}

void Sirion::VkInstanceWrapper::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo {};
    ubo.model = glm::rotate(Matrix4(1.0f), time * glm::radians(90.0f), Vector3(0.0f, 0.0f, 1.0f));
    ubo.view  = glm::lookAt(Vector3(2.0f, 2.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
    ubo.proj =
        glm::perspective(glm::radians(45.0f), m_swapChainExtent.width / (float)m_swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Sirion::VkInstanceWrapper::createDescriptorPool() {
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes(2);
    descriptorPoolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(m_swapChainImages.size());
    // For the allocation of the combined image sampler
    descriptorPoolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(m_swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    poolInfo.pPoolSizes    = descriptorPoolSizes.data();
    poolInfo.maxSets       = static_cast<uint32_t>(m_swapChainImages.size());

    if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Sirion::VkInstanceWrapper::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(m_swapChainImages.size(), m_descriptorSetLayout);
    VkDescriptorSetAllocateInfo        allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_swapChainImages.size());
    allocInfo.pSetLayouts        = layouts.data();

    m_descriptorSets.resize(m_swapChainImages.size());
    if (vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = m_uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(UniformBufferObject);

        // The resources for a combined image sampler structure must be specified in a VkDescriptorImageInfo struct
        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = m_textureImageView;
        imageInfo.sampler     = m_textureSampler;


        std::vector<VkWriteDescriptorSet> descriptorWrites(2);
        descriptorWrites[0].dstSet = m_descriptorSets[i];
        // Give our uniform buffer binding index 0
        descriptorWrites[0].dstBinding = 0;
        // Specify the first index in the array of descriptors that we want to update.
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // Specify how many array elements you want to update.
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo     = &bufferInfo;

        descriptorWrites[1].dstSet          = m_descriptorSets[i];
        descriptorWrites[1].dstBinding      = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo      = &imageInfo;


        vkUpdateDescriptorSets(m_logicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}

void Sirion::VkInstanceWrapper::createCommandBuffers() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);
    m_commandBuffers.resize(m_swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = m_commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

        for (size_t i = 0; i < m_commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.flags                    = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass            = m_renderPass;
        renderPassInfo.framebuffer           = m_swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset     = VkOffset2D {0, 0};
        renderPassInfo.renderArea.extent     = m_swapChainExtent;

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color           = m_clear_color;
        clearValues[1].depthStencil    = VkClearDepthStencilValue {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        // Bind the compute pipeline
        // vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelinePhysics);
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineResetCellVertex);

        // Bind descriptor sets for compute
         vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1,
         m_computeDescriptorSet.data(), 0, nullptr);

        // Dispatch the compute kernel, with one thread for each vertex
         vkCmdDispatch(m_commandBuffers[i], m_num_grid_cells, 1, 1);

        VkBufferMemoryBarrier computeToComputeBarrier = {};
         computeToComputeBarrier.srcAccessMask =
             VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier.dstAccessMask =
             VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier.srcQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier.dstQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier.buffer                = m_cellVertCountBuffer;
        computeToComputeBarrier.offset                = 0;
        computeToComputeBarrier.size                  = m_num_grid_cells * sizeof(int);

        VkPipelineStageFlags computeShaderStageFlags_1(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        VkPipelineStageFlags computeShaderStageFlags_2(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        vkCmdPipelineBarrier(m_commandBuffers[i],
                             computeShaderStageFlags_1,
                             computeShaderStageFlags_2,
                             VkDependencyFlags(),
                             0,
                             nullptr,
                             1,
                             &computeToComputeBarrier,
                             0,
                             nullptr);

        // Bind the compute pipeline
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineFillCellVertex);

        // Bind descriptor sets for compute
         vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1,
                                m_computeDescriptorSet.data(),
                                0,
                                nullptr);

        // Dispatch the compute kernel, with one thread for each vertex
         vkCmdDispatch(m_commandBuffers[i], uint32_t(m_raw_verts.size()), 1, 1);

        VkBufferMemoryBarrier computeToComputeBarrier1 = {};
         computeToComputeBarrier1.srcAccessMask =
             VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier1.dstAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier1.srcQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier1.dstQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier1.buffer                = m_cellVertCountBuffer;
        computeToComputeBarrier1.offset                = 0;
        computeToComputeBarrier1.size                  = m_num_grid_cells * sizeof(int); // vertexBufferSize

        VkPipelineStageFlags computeShaderStageFlags_3(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        VkPipelineStageFlags computeShaderStageFlags_4(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        vkCmdPipelineBarrier(m_commandBuffers[i],
                             computeShaderStageFlags_3,
                             computeShaderStageFlags_4,
                             VkDependencyFlags(),
                             0,
                             nullptr,
                             1,
                             &computeToComputeBarrier1,
                             0,
                             nullptr);

        // Bind the compute pipeline
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelinePhysics);

        // Bind descriptor sets for compute
        vkCmdBindDescriptorSets(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1,
                                m_computeDescriptorSet.data(),
                                0,
                                nullptr);

        // Dispatch the compute kernel, with one thread for each vertex
        vkCmdDispatch(m_commandBuffers[i], uint32_t(m_raw_verts.size()), 1, 1);

        VkBufferMemoryBarrier computeToComputeBarrier2 = {};
        computeToComputeBarrier2.srcAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier2.dstAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToComputeBarrier2.srcQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier2.dstQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToComputeBarrier2.buffer                = m_vertexBuffer2;
        computeToComputeBarrier2.offset                = 0;
        computeToComputeBarrier2.size = uint32_t(m_raw_verts.size()) * sizeof(Vertex); // vertexBufferSize

        VkPipelineStageFlags computeShaderStageFlags_5(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        VkPipelineStageFlags computeShaderStageFlags_6(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

        vkCmdPipelineBarrier(m_commandBuffers[i],
                             computeShaderStageFlags_5,
                             computeShaderStageFlags_6,
                             VkDependencyFlags(),
                             0,
                             nullptr,
                             1,
                             &computeToComputeBarrier2,
                             0,
                             nullptr);

        // Bind the compute pipeline
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineSphereVertex);

        // Bind descriptor sets for compute
        vkCmdBindDescriptorSets(m_commandBuffers[i],
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                m_computePipelineLayout,
                                0,
                                1,
                                m_computeDescriptorSet.data(),
                                0,
                                nullptr);

        // Dispatch the compute kernel, with one thread for each vertex
        vkCmdDispatch(m_commandBuffers[i], uint32_t(m_sphere_verts.size()), 1, 1);


        // Define a memory barrier to transition the vertex buffer from a compute storage object to a vertex input
        VkBufferMemoryBarrier computeToVertexBarrier = {};
        computeToVertexBarrier.srcAccessMask =
            VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        computeToVertexBarrier.dstAccessMask         = VkAccessFlagBits::VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        computeToVertexBarrier.srcQueueFamilyIndex   = queueFamilyIndices.computeFamily.value();
        computeToVertexBarrier.dstQueueFamilyIndex   = queueFamilyIndices.graphicsFamily.value();
        computeToVertexBarrier.buffer                = m_sphereVertsBuffer;
        computeToVertexBarrier.offset                = 0;
        computeToVertexBarrier.size = uint32_t(m_sphere_verts.size()) * sizeof(Vertex); // vertexBufferSize

        VkPipelineStageFlags computeShaderStageFlags(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        VkPipelineStageFlags vertexShaderStageFlags(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

        vkCmdPipelineBarrier(m_commandBuffers[i],
                             computeShaderStageFlags,
                             vertexShaderStageFlags,
                             VkDependencyFlags(),
                             0,
                             nullptr,
                             1,
                             &computeToVertexBarrier,
                             0,
                             nullptr);

        vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

        VkBuffer     vertexBuffers[] = {m_sphereVertsBuffer};
        VkDeviceSize offsets[]       = {0};
        vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(m_commandBuffers[i], m_indexBuffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(
            m_commandBuffers[i], VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[i], 0, nullptr);

        vkCmdDrawIndexed(m_commandBuffers[i], static_cast<uint32_t>(uint32_t(m_sphere_indices.size())), 1, 0, 0, 0);

        vkCmdEndRenderPass(m_commandBuffers[i]);

        if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }

    }

}

void Sirion::VkInstanceWrapper::createSyncObjects() {
    m_imageAvailableSemaphores.resize(m_max_frames_in_flight);
    m_renderFinishedSemaphores.resize(m_max_frames_in_flight);
    m_inFlightFences.resize(m_max_frames_in_flight);

    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < m_max_frames_in_flight; i++)
    {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }


}

void Sirion::VkInstanceWrapper::cleanupSwapChain() {
    vkDestroyImageView(m_logicalDevice, m_depthImageView, nullptr);
    vkDestroyImage(m_logicalDevice, m_depthImage, nullptr);
    vkFreeMemory(m_logicalDevice, m_depthImageMemory, nullptr);


    for (auto framebuffer : m_swapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_logicalDevice, framebuffer, nullptr);
    }
    vkFreeCommandBuffers(m_logicalDevice, m_commandPool, m_commandBuffers.size(), m_commandBuffers.data());

    vkDestroyPipeline(m_logicalDevice, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr);


    for (auto imageView : m_swapChainImageViews)
    {
        vkDestroyImageView(m_logicalDevice, imageView, nullptr);
    }

    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        vkDestroyBuffer(m_logicalDevice, m_uniformBuffers[i], nullptr);
        vkFreeMemory(m_logicalDevice, m_uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr);
    vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
}


Sirion::QueueFamilyIndices Sirion::VkInstanceWrapper::findQueueFamilies(VkPhysicalDevice& device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

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
    setupGraphicsPipeline();

    // ===============

    setupCommandPool();
    setupDepthResources();
    setupFramebuffers();
    setupTextureImage();
    setupTextureImageView();
    setupTextureSampler();

    // loadModel();
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


void Sirion::VkInstanceWrapper::idle() { 
    vkDeviceWaitIdle(m_logicalDevice); }

void Sirion::VkInstanceWrapper::drawFrame(GLFWwindow* window)
{
    vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    uint32_t imageIndex;
    VkResult vr = vkAcquireNextImageKHR(m_logicalDevice,
                                        m_swapChain,
                                        std::numeric_limits<uint64_t>::max(),
                                        m_imageAvailableSemaphores[m_currentFrame],
                                        nullptr,
                                        &imageIndex);
    if (vr != VK_SUCCESS)
    {
        if (vr == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain(window);
            return;
        }
        else
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
    }

    VkDeviceSize bufferSize = static_cast<uint32_t>(m_raw_verts.size() * sizeof(Vertex));
    copyBuffer(m_vertexBuffer2, m_vertexBuffer1, bufferSize);
    updateUniformBuffer(imageIndex);
    VkSubmitInfo submitInfo = {};

    VkSemaphore          waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[]     = {VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount         = 1;
    submitInfo.pWaitSemaphores            = waitSemaphores;
    submitInfo.pWaitDstStageMask          = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &m_commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[]  = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapChain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;

    VkResult rp = vkQueuePresentKHR(m_presentQueue, &presentInfo);
    if (rp != VK_SUCCESS)
    {
        if (rp == VkResult::VK_ERROR_OUT_OF_DATE_KHR)
        {
        }
        else
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    if (rp == VkResult::VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        recreateSwapChain(window);
        return;
    }
    m_currentFrame = (m_currentFrame + 1) % m_max_frames_in_flight;
}

Sirion::VkInstanceWrapper::~VkInstanceWrapper()
{
    // Swap chain has to be destroyed before surface
    cleanupSwapChain();
    vkDestroyPipeline(m_logicalDevice, m_computePipelinePhysics, nullptr);
    vkDestroyPipeline(m_logicalDevice, m_computePipelineFillCellVertex, nullptr);
    vkDestroyPipeline(m_logicalDevice, m_computePipelineResetCellVertex, nullptr);
    vkDestroyPipeline(m_logicalDevice, m_computePipelineSphereVertex, nullptr);
    vkDestroyPipelineLayout(m_logicalDevice, m_computePipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_logicalDevice, m_computeDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_logicalDevice, m_computeDescriptorSetLayout, nullptr);

    // The main texture image is used till the end
    vkDestroySampler(m_logicalDevice, m_textureSampler, nullptr);
    vkDestroyImageView(m_logicalDevice, m_textureImageView, nullptr);
    vkDestroyImage(m_logicalDevice, m_textureImage, nullptr);
    vkFreeMemory(m_logicalDevice, m_textureImageMemory, nullptr);

    vkDestroyBuffer(m_logicalDevice, m_numVertsBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, m_numVertsBufferMemory, nullptr);

    vkDestroyBuffer(m_logicalDevice, m_vertexBuffer1, nullptr);
    vkFreeMemory(m_logicalDevice, m_vertexBufferMemory1, nullptr);

    vkDestroyBuffer(m_logicalDevice, m_vertexBuffer2, nullptr);
    vkFreeMemory(m_logicalDevice, m_vertexBufferMemory2, nullptr);

    vkDestroyBuffer(m_logicalDevice, m_indexBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, m_indexBufferMemory, nullptr);

    vkDestroyBuffer(m_logicalDevice, m_cellVertArrayBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, m_cellVertArrayBufferMemory, nullptr);
    vkDestroyBuffer(m_logicalDevice, m_cellVertCountBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, m_cellVertCountBufferMemory, nullptr);
    vkDestroyBuffer(m_logicalDevice, m_sphereVertsBuffer, nullptr);
    vkFreeMemory(m_logicalDevice, m_sphereVertsBufferMemory, nullptr);

    for (size_t i = 0; i < m_max_frames_in_flight; i++)
    {
        vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_logicalDevice, m_inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyDevice(m_logicalDevice, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    delete m_createInfo.pApplicationInfo;
    delete m_createInfo.ppEnabledLayerNames; // ppEnabledLayerNames is a pointer to heap
}

VkShaderModule Sirion::VkUtils::createShaderModule(VkDevice& device, const std::vector<unsigned char>& shader_code)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shader_code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shader_module) != VK_SUCCESS)
    {
        return VK_NULL_HANDLE;
    }
    return shader_module;
}
