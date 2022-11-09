#include "vulkan_instance.h"

namespace Sirion
{
    bool VulkanInstance::checkValidationLayerSupport()
    {
        auto availableLayers = vk::enumerateInstanceLayerProperties();
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
    std::vector<const char*> VulkanInstance::getRequiredExtensions()
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    void VulkanInstance::createInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        auto appInfo = vk::ApplicationInfo(
            "Title", VK_MAKE_VERSION(1, 0, 0), "No Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

        auto extensions = getRequiredExtensions();

        auto createInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(),
                                                 &appInfo,
                                                 0,
                                                 nullptr,
                                                 static_cast<uint32_t>(extensions.size()),
                                                 extensions.data());

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        try
        {
            m_instance = vk::createInstanceUnique(createInfo, nullptr);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }
    void VulkanInstance::createSurface()
    {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(*m_instance, m_window, nullptr, &rawSurface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }

        m_surface = rawSurface;
    }
    bool VulkanInstance::isDeviceSuitable(const vk::PhysicalDevice& device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        vk::PhysicalDeviceFeatures supportedFeatures;
        device.getFeatures(&supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    QueueFamilyIndices VulkanInstance::findQueueFamilies(vk::PhysicalDevice device)
    {
        QueueFamilyIndices indices;

        auto queueFamilies = device.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
            {
                indices.computeFamily = i;
            }

            if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, m_surface))
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    bool VulkanInstance::checkDeviceExtensionSupport(const vk::PhysicalDevice& device)
    {
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : device.enumerateDeviceExtensionProperties())
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails VulkanInstance::querySwapChainSupport(const vk::PhysicalDevice& device)
    {
        SwapChainSupportDetails details;
        details.capabilities = device.getSurfaceCapabilitiesKHR(m_surface);
        details.formats      = device.getSurfaceFormatsKHR(m_surface);
        details.presentModes = device.getSurfacePresentModesKHR(m_surface);

        return details;
    }

    void VulkanInstance::pickPhysicalDevice()
    {
        auto devices = m_instance->enumeratePhysicalDevices();
        if (devices.size() == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        for (const auto& device : devices)
        {
            if (isDeviceSuitable(device))
            {
                m_physicalDevice = device;
                break;
            }
        }

        if (!m_physicalDevice)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
    void VulkanInstance::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t>                     uniqueQueueFamilies = {
            indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value()};

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back({vk::DeviceQueueCreateFlags(),
                                        queueFamily,
                                        1, // queueCount
                                        &queuePriority});
        }

        auto deviceFeatures              = vk::PhysicalDeviceFeatures();
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        auto createInfo                  = vk::DeviceCreateInfo(
            vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }

        try
        {
            m_device = m_physicalDevice.createDeviceUnique(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        m_graphicsQueue = m_device->getQueue(indices.graphicsFamily.value(), 0);
        m_presentQueue  = m_device->getQueue(indices.presentFamily.value(), 0);
        m_computeQueue  = m_device->getQueue(indices.computeFamily.value(), 0);
    }
    vk::SurfaceFormatKHR
    VulkanInstance::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
        {
            return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
        }

        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    vk::PresentModeKHR
    VulkanInstance::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes)
    {
        vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox)
            {
                return availablePresentMode;
            }
            else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
            {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }
    vk::Extent2D VulkanInstance::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(m_window, &width, &height);

            vk::Extent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width  = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height,
                                           std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
    void VulkanInstance::createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

        vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
        vk::Extent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR createInfo(vk::SwapchainCreateFlagsKHR(),
                                              m_surface,
                                              imageCount,
                                              surfaceFormat.format,
                                              surfaceFormat.colorSpace,
                                              extent,
                                              1, // imageArrayLayers
                                              vk::ImageUsageFlagBits::eColorAttachment);

        QueueFamilyIndices indices              = findQueueFamilies(m_physicalDevice);
        uint32_t           queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode    = presentMode;
        createInfo.clipped        = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        try
        {
            m_swapChain = m_device->createSwapchainKHR(createInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        m_swapChainImages = m_device->getSwapchainImagesKHR(m_swapChain);

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent      = extent;
    }

    vk::ImageView VulkanInstance::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags)

    {
        vk::ImageViewCreateInfo viewInfo {};
        viewInfo.image                           = image;
        viewInfo.viewType                        = vk::ImageViewType::e2D;
        viewInfo.format                          = format;
        viewInfo.subresourceRange.aspectMask     = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VkImageView imageView;
        try
        {
            imageView = m_device->createImageView(viewInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void VulkanInstance::createImageViews()
    {
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            try
            {
                m_swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat);
            }
            catch (vk::SystemError err)
            {
                std::cerr << "failed to create image views!" << std::endl;
            }
        }
    }

    vk::Format VulkanInstance::findSupportedFormat(const std::vector<vk::Format>& candidates,
                                                   vk::ImageTiling                tiling,
                                                   vk::FormatFeatureFlags         features)

    {
        for (vk::Format format : candidates)
        {
            vk::FormatProperties props;
            m_physicalDevice.getFormatProperties(format, &props);

            if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    vk::Format VulkanInstance::findDepthFormat()
    {
        vk::Format depthFormat;
        try
        {
            depthFormat =
                findSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                                    vk::ImageTiling::eOptimal,
                                    vk::FormatFeatureFlags(vk::FormatFeatureFlagBits::eDepthStencilAttachment));
        }
        catch (std::runtime_error err)
        {
            std::cerr << err.what() << std::endl;
            depthFormat = vk::Format::eD32Sfloat;
        }

        return depthFormat;
    }

    void VulkanInstance::createRenderPass()
    {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format                    = m_swapChainImageFormat;
        colorAttachment.samples                   = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp                    = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp                   = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp             = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp            = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout             = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout               = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment              = 0;
        colorAttachmentRef.layout                  = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentDescription depthAttachment {};
        depthAttachment.format         = findDepthFormat();
        depthAttachment.samples        = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp        = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout  = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDescription subpass  = {};
        subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef; // Unlike color attachments, a subpass can only use a
                                                               // single depth (+stencil) attachment.

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass            = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass            = 0;
        dependency.srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        // dependency.srcAccessMask = 0;
        dependency.dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask =
            vk::AccessFlagBits::eDepthStencilAttachmentWrite | vk::AccessFlagBits::eColorAttachmentWrite;

        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount          = static_cast<uint32_t>(attachments.size());
        ;
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        try
        {
            m_renderPass = m_device->createRenderPass(renderPassInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanInstance::createDescriptorSetLayout()
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding {};
        // Specify the binding used in the shader
        uboLayoutBinding.binding = 0;
        // Specify the type of descriptor, which is a uniform buffer object
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        // Our MVP transformation is in a single uniform buffer object
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.stageFlags         = vk::ShaderStageFlagBits::eVertex;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional, used for image sampling related descriptors

        vk::DescriptorSetLayoutBinding samplerLayoutBinding {};
        samplerLayoutBinding.binding            = 1;
        samplerLayoutBinding.descriptorCount    = 1;
        samplerLayoutBinding.descriptorType     = vk::DescriptorType::eCombinedImageSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        // Indicate that we intend to use the combined image sampler descriptor in the fragment shader.
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        vk::DescriptorSetLayoutCreateInfo descriptorLayoutInfo {};
        descriptorLayoutInfo.flags        = vk::DescriptorSetLayoutCreateFlags();
        descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        descriptorLayoutInfo.pBindings    = bindings.data();

        try
        {
            m_descriptorSetLayout = m_device->createDescriptorSetLayout(descriptorLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VulkanInstance::createGraphicsPipeline()
    {
        vk::UniqueShaderModule vertShaderModule = VulkanUtils::createShaderModule(m_device, SHADER_VERT);
        vk::UniqueShaderModule fragShaderModule = VulkanUtils::createShaderModule(m_device, SHADER_VERT);

        vk::PipelineShaderStageCreateInfo shaderStages[] = {
            {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, *vertShaderModule, "main"},
            {vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, *fragShaderModule, "main"}};

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount          = 0;
        vertexInputInfo.vertexAttributeDescriptionCount        = 0;

        auto bindingDescription    = getBindingDescription();
        auto attributeDescriptions = getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount   = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.topology                                 = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable                   = VK_FALSE;

        vk::Viewport viewport = {};
        viewport.x            = 0.0f;
        viewport.y            = 0.0f;
        viewport.width        = (float)m_swapChainExtent.width;
        viewport.height       = (float)m_swapChainExtent.height;
        viewport.minDepth     = 0.0f;
        viewport.maxDepth     = 1.0f;

        vk::Rect2D scissor = {};
        scissor.offset     = vk::Offset2D {0, 0};
        scissor.extent     = m_swapChainExtent;

        vk::PipelineViewportStateCreateInfo viewportState = {};
        viewportState.viewportCount                       = 1;
        viewportState.pViewports                          = &viewport;
        viewportState.scissorCount                        = 1;
        viewportState.pScissors                           = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.depthClampEnable                         = VK_FALSE;
        rasterizer.rasterizerDiscardEnable                  = VK_FALSE;
        rasterizer.polygonMode                              = vk::PolygonMode::eFill;
        rasterizer.lineWidth                                = 1.0f;
        rasterizer.cullMode                                 = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace                                = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable                          = VK_FALSE;

        vk::PipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sampleShadingEnable                    = VK_FALSE;
        multisampling.rasterizationSamples                   = vk::SampleCountFlagBits::e1;

        vk::PipelineDepthStencilStateCreateInfo depthStencil {};
        // The depthTestEnable field specifies if the depth of new fragments should be compared to the depth buffer
        // to see if they should be discarded.
        depthStencil.depthTestEnable  = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        // Specify the comparison that is performed to keep or discard fragments.
        depthStencil.depthCompareOp        = vk::CompareOp::eLess;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds        = 0.0f; // Optional
        depthStencil.maxDepthBounds        = 1.0f; // Optional
        depthStencil.stencilTestEnable     = VK_FALSE;
        // depthStencil.front = {}; // Optional
        // depthStencil.back = {}; // Optional

        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.logicOpEnable                         = VK_FALSE;
        colorBlending.logicOp                               = vk::LogicOp::eCopy;
        colorBlending.attachmentCount                       = 1;
        colorBlending.pAttachments                          = &colorBlendAttachment;
        colorBlending.blendConstants[0]                     = 0.0f;
        colorBlending.blendConstants[1]                     = 0.0f;
        colorBlending.blendConstants[2]                     = 0.0f;
        colorBlending.blendConstants[3]                     = 0.0f;

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.setLayoutCount         = 1;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pSetLayouts            = &m_descriptorSetLayout;

        try
        {
            m_pipelineLayout = m_device->createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount                     = 2;
        pipelineInfo.pStages                        = shaderStages;
        pipelineInfo.pVertexInputState              = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState            = &inputAssembly;
        pipelineInfo.pViewportState                 = &viewportState;
        pipelineInfo.pRasterizationState            = &rasterizer;
        pipelineInfo.pMultisampleState              = &multisampling;
        pipelineInfo.pColorBlendState               = &colorBlending;
        pipelineInfo.pDepthStencilState             = &depthStencil;
        pipelineInfo.layout                         = m_pipelineLayout;
        pipelineInfo.renderPass                     = m_renderPass;
        pipelineInfo.subpass                        = 0;
        pipelineInfo.basePipelineHandle             = nullptr;

        try
        {
            m_graphicsPipeline = m_device->createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
    }

    void VulkanInstance::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex          = queueFamilyIndices.graphicsFamily.value();

        try
        {
            m_commandPool = m_device->createCommandPool(poolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    uint32_t VulkanInstance::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = m_physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void VulkanInstance::createImage(uint32_t                width,
                                     uint32_t                height,
                                     vk::Format              format,
                                     vk::ImageTiling         tiling,
                                     vk::ImageUsageFlags     usage,
                                     vk::MemoryPropertyFlags properties,
                                     vk::Image&              image,
                                     vk::DeviceMemory&       imageMemory)
    {
        // vk::Format texFormat = vk::Format::eR8G8B8A8Srgb;
        vk::ImageCreateInfo imgInfo({},
                                    vk::ImageType::e2D,
                                    format,
                                    {width, height, 1},
                                    1,
                                    1,
                                    vk::SampleCountFlagBits::e1,
                                    tiling,
                                    usage,
                                    vk::SharingMode::eExclusive,
                                    0,
                                    nullptr,
                                    vk::ImageLayout::eUndefined);

        try
        {
            image = m_device->createImage(imgInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create image!");
        }

        vk::MemoryRequirements memRequirements;
        m_device->getImageMemoryRequirements(image, &memRequirements);

        vk::MemoryAllocateInfo allocInfo {};
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        try
        {
            imageMemory = m_device->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        m_device->bindImageMemory(image, imageMemory, 0);
    }


    void VulkanInstance::createDepthResources()
    {
        // Find the depth format first
        vk::Format depthFormat = findDepthFormat();
        ;
        vk::ImageCreateInfo depthImgInfo({},
                                         vk::ImageType::e2D,
                                         depthFormat,
                                         {m_swapChainExtent.width, m_swapChainExtent.height, 1},
                                         1,
                                         1,
                                         vk::SampleCountFlagBits::e1,
                                         vk::ImageTiling::eOptimal,
                                         vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                         vk::SharingMode::eExclusive,
                                         0,
                                         nullptr,
                                         vk::ImageLayout::eUndefined);

        createImage(m_swapChainExtent.width,
                    m_swapChainExtent.height,
                    depthFormat,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlags(vk::ImageUsageFlagBits::eDepthStencilAttachment),
                    vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal),
                    m_depthImage,
                    m_depthImageMemory);

        m_depthImageView =
            createImageView(m_depthImage, depthFormat, vk::ImageAspectFlags(vk::ImageAspectFlagBits::eDepth));

        // The undefined layout can be used as initial layout
        // transitionImageLayout(depthImage, depthFormat, vk::ImageLayout::eUndefined,
        // vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    void VulkanInstance::createFramebuffers()
    {
        m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            std::array<vk::ImageView, 2> attachments = {m_swapChainImageViews[i], m_depthImageView};

            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.renderPass                = m_renderPass;
            framebufferInfo.attachmentCount           = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments              = attachments.data();
            framebufferInfo.width                     = m_swapChainExtent.width;
            framebufferInfo.height                    = m_swapChainExtent.height;
            framebufferInfo.layers                    = 1;

            try
            {
                m_swapChainFramebuffers[i] = m_device->createFramebuffer(framebufferInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanInstance::createBuffer(vk::DeviceSize          size,
                                      vk::BufferUsageFlags    usage,
                                      vk::MemoryPropertyFlags properties,
                                      vk::Buffer&             buffer,
                                      vk::DeviceMemory&       bufferMemory)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size                 = size;
        bufferInfo.usage                = usage;
        // bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        try
        {
            buffer = m_device->createBuffer(bufferInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        vk::MemoryRequirements memRequirements = m_device->getBufferMemoryRequirements(buffer);

        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize         = memRequirements.size;
        allocInfo.memoryTypeIndex        = findMemoryType(memRequirements.memoryTypeBits, properties);

        try
        {
            bufferMemory = m_device->allocateMemory(allocInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        m_device->bindBufferMemory(buffer, bufferMemory, 0);
    }

    void VulkanInstance::transitionImageLayout(vk::Image       image,
                                               vk::Format      format,
                                               vk::ImageLayout oldLayout,
                                               vk::ImageLayout newLayout)
    {
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        // Use an image memory barrier to perform layout transitions, which is primary for synchronization purposes
        vk::ImageMemoryBarrier barrier {};
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        // Not want to use the barrier to transfer queue family ownership
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        // barrier.image and barrier.subresourceRange specify the image that is affected and the specific part of
        // the image
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        vk::PipelineStageFlags sourceStage;
        vk::PipelineStageFlags destinationStage;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlags(0);
            barrier.dstAccessMask = vk::AccessFlags(vk::AccessFlagBits::eTransferWrite);

            sourceStage      = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTopOfPipe);
            destinationStage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer);
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
                 newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlags(vk::AccessFlagBits::eTransferWrite);
            barrier.dstAccessMask = vk::AccessFlags(vk::AccessFlagBits::eShaderRead);

            sourceStage      = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer);
            destinationStage = vk::PipelineStageFlags(vk::PipelineStageFlagBits::eFragmentShader);
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

        endSingleTimeCommands(commandBuffer);
    }

    vk::CommandBuffer VulkanInstance::beginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo {};
        allocInfo.level              = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool        = m_commandPool;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer;
        m_device->allocateCommandBuffers(&allocInfo, &commandBuffer);

        vk::CommandBufferBeginInfo beginInfo {};
        beginInfo.flags = vk::CommandBufferUsageFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        commandBuffer.begin(&beginInfo);
        return commandBuffer;
    }

    void VulkanInstance::endSingleTimeCommands(vk::CommandBuffer& commandBuffer)
    {
        commandBuffer.end();

        vk::SubmitInfo submitInfo {};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;

        m_graphicsQueue.submit(submitInfo, nullptr);
        m_graphicsQueue.waitIdle();

        m_device->freeCommandBuffers(m_commandPool, 1, &commandBuffer);
    }

    void VulkanInstance::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
    {
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::BufferImageCopy region {};
        region.bufferOffset      = 0;
        region.bufferRowLength   = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;

        region.imageOffset = vk::Offset3D {0, 0, 0};
        region.imageExtent = vk::Extent3D {width, height, 1};

        // Buffer to image copy operations are enqueued using the following function
        commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

        endSingleTimeCommands(commandBuffer);
    }


    void VulkanInstance::createTextureImage()
    {
        int            texWidth, texHeight, texChannels;
        stbi_uc*       pixels = stbi_load(m_texture_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        vk::DeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            std::cerr << "failed to load texture image!" << std::endl;
        }
        else
        {
            std::cout << "load texture image successfully!" << std::endl;
        }

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;

        try
        {
            createBuffer(imageSize,
                         vk::BufferUsageFlagBits::eTransferSrc,
                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                         stagingBuffer,
                         stagingBufferMemory);
        }
        catch (std::runtime_error err)
        {
            std::cerr << err.what() << std::endl;
        }

        void* data = m_device->mapMemory(stagingBufferMemory, 0, imageSize);
        memcpy(data, pixels, (size_t)imageSize);
        m_device->unmapMemory(stagingBufferMemory);

        // Clean up the original pixel array
        stbi_image_free(pixels);

        // Create the texture image
        try
        {
            createImage(texWidth,
                        texHeight,
                        vk::Format::eR8G8B8A8Srgb,
                        vk::ImageTiling::eOptimal,
                        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
                        vk::MemoryPropertyFlags(vk::MemoryPropertyFlagBits::eDeviceLocal),
                        m_textureImage,
                        m_textureImageMemory);
        }
        catch (std::runtime_error err)
        {
            std::cerr << err.what() << std::endl;
        }

        // Copy the staging buffer to the texture image:
        // - Transition the texture image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        // - Execute the buffer to image copy operation
        transitionImageLayout(m_textureImage,
                              vk::Format::eR8G8B8A8Srgb,
                              vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eTransferDstOptimal);
        copyBufferToImage(
            stagingBuffer, m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

        // To be able to start sampling from the texture image in the shader, use one last transition to prepare it
        // for shader access:
        transitionImageLayout(m_textureImage,
                              vk::Format::eR8G8B8A8Srgb,
                              vk::ImageLayout::eTransferDstOptimal,
                              vk::ImageLayout::eShaderReadOnlyOptimal);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createTextureImageView()
    {
        m_textureImageView = createImageView(m_textureImage, vk::Format::eR8G8B8A8Srgb);
    }

    void VulkanInstance::createTextureSampler()
    {
        // Samplers are configured through a vk::SamplerCreateInfo structure,
        // which specifies all filters and transformations that it should apply.
        vk::SamplerCreateInfo samplerInfo {};

        // Specify how to interpolate texels that are magnified or minified
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;

        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy =
            16.0f; // limits the amount of texel samples that can be used to calculate the final color

        // Return black when sampling beyond the image with clamp to border addressing mode.
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;

        // The texels are addressed using the [0, 1) range on all axes.
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp     = vk::CompareOp::eAlways;

        // Use mipmapping
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod     = 0.0f;
        samplerInfo.maxLod     = 0.0f;

        try
        {
            // The sampler is a distinct object that provides an interface to extract colors from a texture.
            m_textureSampler = m_device->createSampler(samplerInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void VulkanInstance::copyBuffer(vk::Buffer& srcBuffer, vk::Buffer& dstBuffer, const vk::DeviceSize& size)
    {
        vk::CommandBuffer commandBuffer = beginSingleTimeCommands();

        vk::BufferCopy copyRegion {};
        copyRegion.size = size;
        commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void VulkanInstance::createVertexBuffers()
    {
        vk::DeviceSize bufferSize = static_cast<uint32_t>(m_particles.m_raw_verts.size() * sizeof(Vertex));
        // vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        void* data = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, m_particles.m_raw_verts.data(), (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        // Create buffer for the old vertices
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_vertexBuffer1,
                     m_vertexBufferMemory1);

        // Create buffer for the new vertices
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_vertexBuffer2,
                     m_vertexBufferMemory2);

        copyBuffer(stagingBuffer, m_vertexBuffer1, bufferSize);
        copyBuffer(stagingBuffer, m_vertexBuffer2, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createIndexBuffer()
    {
        vk::DeviceSize bufferSize =
            static_cast<uint32_t>(m_particles.m_sphere_indices.size() * sizeof(m_particles.m_sphere_indices[0]));
        // vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();;

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        void* data = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, m_particles.m_sphere_indices.data(), (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_indexBuffer,
                     m_indexBufferMemory);

        copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createNumVertsBuffer()
    {
        vk::DeviceSize bufferSize = static_cast<uint32_t>(sizeof(int));

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        const int NUM_VERTS = m_particles.m_raw_verts.size();
        void*     data      = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, &NUM_VERTS, (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        // Create buffer for the old vertices
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent |
                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_numVertsBuffer,
                     m_numVertsBufferMemory);

        copyBuffer(stagingBuffer, m_numVertsBuffer, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createCellVertArrayBuffer()
    {
        vk::DeviceSize bufferSize = static_cast<uint32_t>(sizeof(int) * N_GRID_CELLS * 6);
        // vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();;

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        void* data = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, m_cellVertArray, (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_cellVertArrayBuffer,
                     m_cellVertArrayBufferMemory);

        copyBuffer(stagingBuffer, m_cellVertArrayBuffer, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createCellVertCountBuffer()
    {
        vk::DeviceSize bufferSize = static_cast<uint32_t>(sizeof(int) * N_GRID_CELLS);
        // vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();;

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        void* data = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, m_cellVertCount, (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_cellVertCountBuffer,
                     m_cellVertCountBufferMemory);

        copyBuffer(stagingBuffer, m_cellVertCountBuffer, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createSphereVertsBuffer()
    {
        vk::DeviceSize bufferSize = static_cast<uint32_t>(sizeof(Vertex) * m_particles.m_sphere_verts.size());

        vk::Buffer       stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     stagingBuffer,
                     stagingBufferMemory);

        void* data = m_device->mapMemory(stagingBufferMemory, 0, bufferSize);
        memcpy(data, m_particles.m_sphere_verts.data(), (size_t)bufferSize);
        m_device->unmapMemory(stagingBufferMemory);

        createBuffer(bufferSize,
                     vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal,
                     m_sphereVertsBuffer,
                     m_sphereVertsBufferMemory);

        copyBuffer(stagingBuffer, m_sphereVertsBuffer, bufferSize);

        m_device->destroyBuffer(stagingBuffer);
        m_device->freeMemory(stagingBufferMemory);
    }

    void VulkanInstance::createComputePipeline(std::string computeShaderPath, vk::Pipeline& pipelineIdx)
    {
        auto computeShaderCode   = readFile(computeShaderPath);
        auto computeShaderModule = createShaderModule(computeShaderCode);

        vk::PipelineShaderStageCreateInfo computeShaderStageInfo = {};
        computeShaderStageInfo.flags                             = vk::PipelineShaderStageCreateFlags();
        computeShaderStageInfo.stage                             = vk::ShaderStageFlagBits::eCompute;
        computeShaderStageInfo.module                            = *computeShaderModule;
        computeShaderStageInfo.pName                             = "main";

        vk::DescriptorSetLayoutBinding computeLayoutBindingVertices1 {};
        computeLayoutBindingVertices1.binding            = 0;
        computeLayoutBindingVertices1.descriptorCount    = 1;
        computeLayoutBindingVertices1.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingVertices1.pImmutableSamplers = nullptr;
        computeLayoutBindingVertices1.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        vk::DescriptorSetLayoutBinding computeLayoutBindingVertices2 {};
        computeLayoutBindingVertices2.binding            = 1;
        computeLayoutBindingVertices2.descriptorCount    = 1;
        computeLayoutBindingVertices2.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingVertices2.pImmutableSamplers = nullptr;
        computeLayoutBindingVertices2.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        vk::DescriptorSetLayoutBinding computeLayoutBindingCellVertexArray {};
        computeLayoutBindingCellVertexArray.binding            = 2;
        computeLayoutBindingCellVertexArray.descriptorCount    = 1;
        computeLayoutBindingCellVertexArray.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingCellVertexArray.pImmutableSamplers = nullptr;
        computeLayoutBindingCellVertexArray.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        vk::DescriptorSetLayoutBinding computeLayoutBindingCellVertexCount {};
        computeLayoutBindingCellVertexCount.binding            = 3;
        computeLayoutBindingCellVertexCount.descriptorCount    = 1;
        computeLayoutBindingCellVertexCount.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingCellVertexCount.pImmutableSamplers = nullptr;
        computeLayoutBindingCellVertexCount.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        vk::DescriptorSetLayoutBinding computeLayoutBindingNumVerts {};
        computeLayoutBindingNumVerts.binding            = 4;
        computeLayoutBindingNumVerts.descriptorCount    = 1;
        computeLayoutBindingNumVerts.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingNumVerts.pImmutableSamplers = nullptr;
        computeLayoutBindingNumVerts.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        vk::DescriptorSetLayoutBinding computeLayoutBindingSphereVerts {};
        computeLayoutBindingSphereVerts.binding            = 5;
        computeLayoutBindingSphereVerts.descriptorCount    = 1;
        computeLayoutBindingSphereVerts.descriptorType     = vk::DescriptorType::eStorageBuffer;
        computeLayoutBindingSphereVerts.pImmutableSamplers = nullptr;
        computeLayoutBindingSphereVerts.stageFlags         = vk::ShaderStageFlagBits::eCompute;

        std::vector<vk::DescriptorSetLayoutBinding> bindings = {computeLayoutBindingVertices1,
                                                                computeLayoutBindingVertices2,
                                                                computeLayoutBindingCellVertexArray,
                                                                computeLayoutBindingCellVertexCount,
                                                                computeLayoutBindingNumVerts,
                                                                computeLayoutBindingSphereVerts};

        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        // descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.flags        = vk::DescriptorSetLayoutCreateFlags();
        descriptorSetLayoutCreateInfo.pNext        = nullptr;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        descriptorSetLayoutCreateInfo.pBindings    = bindings.data();

        try
        {
            m_computeDescriptorSetLayout = m_device->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        // VkDescriptorPoolSize poolSizes[1];
        std::array<vk::DescriptorPoolSize, 1> poolSizes {};
        poolSizes[0].type            = vk::DescriptorType::eStorageBuffer;
        poolSizes[0].descriptorCount = 10;

        vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
        // descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.flags         = vk::DescriptorPoolCreateFlags();
        descriptorPoolInfo.pNext         = nullptr;
        descriptorPoolInfo.poolSizeCount = 1;
        descriptorPoolInfo.pPoolSizes    = poolSizes.data();
        descriptorPoolInfo.maxSets       = 1;

        try
        {
            m_computeDescriptorPool = m_device->createDescriptorPool(descriptorPoolInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create compute descriptor pool!");
        }

        vk::DescriptorSetAllocateInfo allocInfo = {};
        // allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = m_computeDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &m_computeDescriptorSetLayout;

        // computeDescriptorSet.resize(1);
        try
        {
            m_computeDescriptorSet = m_device->allocateDescriptorSets(allocInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create compute descriptor sets!");
        }

        // Set descriptor set for the old vertices
        vk::DescriptorBufferInfo computeBufferInfoVertices1 = {};
        computeBufferInfoVertices1.buffer                   = m_vertexBuffer1;
        computeBufferInfoVertices1.offset                   = 0;
        computeBufferInfoVertices1.range                    = static_cast<uint32_t>(m_particles.m_raw_verts.size() * sizeof(Vertex));

        vk::WriteDescriptorSet writeComputeInfoVertices1 = {};
        writeComputeInfoVertices1.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoVertices1.dstBinding             = 0;
        writeComputeInfoVertices1.descriptorCount        = 1;
        writeComputeInfoVertices1.dstArrayElement        = 0;
        writeComputeInfoVertices1.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoVertices1.pBufferInfo            = &computeBufferInfoVertices1;

        // Set descriptor set for the new vertices
        vk::DescriptorBufferInfo computeBufferInfoVertices2 = {};
        computeBufferInfoVertices2.buffer                   = m_vertexBuffer2;
        computeBufferInfoVertices2.offset                   = 0;
        computeBufferInfoVertices2.range = static_cast<uint32_t>(m_particles.m_raw_verts.size() * sizeof(Vertex));

        vk::WriteDescriptorSet writeComputeInfoVertices2 = {};
        writeComputeInfoVertices2.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoVertices2.dstBinding             = 1;
        writeComputeInfoVertices2.descriptorCount        = 1;
        writeComputeInfoVertices2.dstArrayElement        = 0;
        writeComputeInfoVertices2.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoVertices2.pBufferInfo            = &computeBufferInfoVertices2;

        // Set descriptor set for the cell vertex array
        vk::DescriptorBufferInfo computeBufferInfoCellVertexArray = {};
        computeBufferInfoCellVertexArray.buffer                   = m_cellVertArrayBuffer;
        computeBufferInfoCellVertexArray.offset                   = 0;
        computeBufferInfoCellVertexArray.range = static_cast<uint32_t>(N_GRID_CELLS * 6 * sizeof(int));

        vk::WriteDescriptorSet writeComputeInfoCellVertexArray = {};
        writeComputeInfoCellVertexArray.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoCellVertexArray.dstBinding             = 2;
        writeComputeInfoCellVertexArray.descriptorCount        = 1;
        writeComputeInfoCellVertexArray.dstArrayElement        = 0;
        writeComputeInfoCellVertexArray.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoCellVertexArray.pBufferInfo            = &computeBufferInfoCellVertexArray;

        // Set descriptor set for the cell vertex count
        vk::DescriptorBufferInfo computeBufferInfoCellVertexCount = {};
        computeBufferInfoCellVertexCount.buffer                   = m_cellVertCountBuffer;
        computeBufferInfoCellVertexCount.offset                   = 0;
        computeBufferInfoCellVertexCount.range                    = static_cast<uint32_t>(N_GRID_CELLS * sizeof(int));

        vk::WriteDescriptorSet writeComputeInfoCellVertexCount = {};
        writeComputeInfoCellVertexCount.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoCellVertexCount.dstBinding             = 3;
        writeComputeInfoCellVertexCount.descriptorCount        = 1;
        writeComputeInfoCellVertexCount.dstArrayElement        = 0;
        writeComputeInfoCellVertexCount.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoCellVertexCount.pBufferInfo            = &computeBufferInfoCellVertexCount;

        // Set descriptor set for the buffer representing the number of vertices
        vk::DescriptorBufferInfo computeBufferInfoNumVerts = {};
        computeBufferInfoNumVerts.buffer                   = m_numVertsBuffer;
        computeBufferInfoNumVerts.offset                   = 0;
        computeBufferInfoNumVerts.range                    = static_cast<uint32_t>(sizeof(int));

        vk::WriteDescriptorSet writeComputeInfoNumVerts = {};
        writeComputeInfoNumVerts.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoNumVerts.dstBinding             = 4;
        writeComputeInfoNumVerts.descriptorCount        = 1;
        writeComputeInfoNumVerts.dstArrayElement        = 0;
        writeComputeInfoNumVerts.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoNumVerts.pBufferInfo            = &computeBufferInfoNumVerts;

        // Set descriptor set for the buffer representing the sphere vertices
        vk::DescriptorBufferInfo computeBufferInfoSphereVerts = {};
        computeBufferInfoSphereVerts.buffer                   = m_sphereVertsBuffer;
        computeBufferInfoSphereVerts.offset                   = 0;
        computeBufferInfoSphereVerts.range = static_cast<uint32_t>(sizeof(Vertex) * m_particles.m_sphere_verts.size());

        vk::WriteDescriptorSet writeComputeInfoSphereVerts = {};
        writeComputeInfoSphereVerts.dstSet                 = m_computeDescriptorSet[0];
        writeComputeInfoSphereVerts.dstBinding             = 5;
        writeComputeInfoSphereVerts.descriptorCount        = 1;
        writeComputeInfoSphereVerts.dstArrayElement        = 0;
        writeComputeInfoSphereVerts.descriptorType         = vk::DescriptorType::eStorageBuffer;
        writeComputeInfoSphereVerts.pBufferInfo            = &computeBufferInfoSphereVerts;

        std::array<vk::WriteDescriptorSet, 6> writeDescriptorSets = {writeComputeInfoVertices1,
                                                                     writeComputeInfoVertices2,
                                                                     writeComputeInfoCellVertexArray,
                                                                     writeComputeInfoCellVertexCount,
                                                                     writeComputeInfoNumVerts,
                                                                     writeComputeInfoSphereVerts};
        m_device->updateDescriptorSets(
            static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts = {m_computeDescriptorSetLayout};

        vk::PipelineLayoutCreateInfo computePipelineLayoutInfo = {};
        computePipelineLayoutInfo.flags                        = vk::PipelineLayoutCreateFlags();
        computePipelineLayoutInfo.setLayoutCount               = static_cast<uint32_t>(descriptorSetLayouts.size());
        computePipelineLayoutInfo.pSetLayouts                  = descriptorSetLayouts.data();
        computePipelineLayoutInfo.pushConstantRangeCount       = 0;
        computePipelineLayoutInfo.pPushConstantRanges          = 0;

        try
        {
            m_computePipelineLayout = m_device->createPipelineLayout(computePipelineLayoutInfo);
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        vk::ComputePipelineCreateInfo computePipelineInfo = {};
        computePipelineInfo.flags                         = vk::PipelineCreateFlags();
        computePipelineInfo.stage                         = computeShaderStageInfo;
        computePipelineInfo.layout                        = m_computePipelineLayout;

        try
        {
            pipelineIdx = m_device->createComputePipeline(nullptr, computePipelineInfo).value;
        }
        catch (vk::SystemError err)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }
    }








} // namespace Sirion