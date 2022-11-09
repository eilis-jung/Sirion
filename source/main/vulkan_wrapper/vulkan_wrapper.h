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
#include "math/vertex.h"
#include "particles/particle_generator.h"

namespace Sirion
{
    class VulkanSpheresRenderer
    {
    public:
        void run()
        {
            initVulkan();
            cleanup();
        }

    private:
        

        // Texture image and view
        vk::Image        textureImage;
        vk::DeviceMemory textureImageMemory;
        vk::ImageView    textureImageView;
        vk::Sampler      textureSampler;

        // Depth image and view
        vk::Image        depthImage;
        vk::DeviceMemory depthImageMemory;
        vk::ImageView    depthImageView;

        

        

        // Vertex* raw_verts = new Vertex[N_FOR_VIS];
        // uint32_t* raw_indices = new uint32_t[N_FOR_VIS];

        int* cellVertArray = new int[N_GRID_CELLS * 6] {0};
        int* cellVertCount = new int[N_GRID_CELLS] {0};


        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;

        

        bool framebufferResized = false;

        void initVulkan()
        {
            
            

            
            
            

            createComputePipeline("../../../source/shader/generated/physicsCompute.spv", computePipelinePhysics);
            createComputePipeline("../../../source/shader/generated/fillCellVertexInfo.spv", computePipelineFillCellVertex);
            createComputePipeline("../../../source/shader/generated/resetCellVertexInfo.spv", computePipelineResetCellVertex);
            createComputePipeline("../../../source/shader/generated/sphereVertexCompute.spv", computePipelineSphereVertex);

            createuniformUboBuffers();
            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();
            createSyncObjects();
        }

        
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
                computeDescriptorSetLayout = device->createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
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
                computeDescriptorPool = device->createDescriptorPool(descriptorPoolInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create compute descriptor pool!");
            }

            vk::DescriptorSetAllocateInfo allocInfo = {};
            // allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool     = computeDescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts        = &computeDescriptorSetLayout;

            // computeDescriptorSet.resize(1);
            try
            {
                computeDescriptorSet = device->allocateDescriptorSets(allocInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create compute descriptor sets!");
            }

            // Set descriptor set for the old vertices
            vk::DescriptorBufferInfo computeBufferInfoVertices1 = {};
            computeBufferInfoVertices1.buffer                   = vertexBuffer1;
            computeBufferInfoVertices1.offset                   = 0;
            computeBufferInfoVertices1.range = static_cast<uint32_t>(raw_verts.size() * sizeof(Vertex));

            vk::WriteDescriptorSet writeComputeInfoVertices1 = {};
            writeComputeInfoVertices1.dstSet                 = computeDescriptorSet[0];
            writeComputeInfoVertices1.dstBinding             = 0;
            writeComputeInfoVertices1.descriptorCount        = 1;
            writeComputeInfoVertices1.dstArrayElement        = 0;
            writeComputeInfoVertices1.descriptorType         = vk::DescriptorType::eStorageBuffer;
            writeComputeInfoVertices1.pBufferInfo            = &computeBufferInfoVertices1;

            // Set descriptor set for the new vertices
            vk::DescriptorBufferInfo computeBufferInfoVertices2 = {};
            computeBufferInfoVertices2.buffer                   = vertexBuffer2;
            computeBufferInfoVertices2.offset                   = 0;
            computeBufferInfoVertices2.range = static_cast<uint32_t>(raw_verts.size() * sizeof(Vertex));

            vk::WriteDescriptorSet writeComputeInfoVertices2 = {};
            writeComputeInfoVertices2.dstSet                 = computeDescriptorSet[0];
            writeComputeInfoVertices2.dstBinding             = 1;
            writeComputeInfoVertices2.descriptorCount        = 1;
            writeComputeInfoVertices2.dstArrayElement        = 0;
            writeComputeInfoVertices2.descriptorType         = vk::DescriptorType::eStorageBuffer;
            writeComputeInfoVertices2.pBufferInfo            = &computeBufferInfoVertices2;

            // Set descriptor set for the cell vertex array
            vk::DescriptorBufferInfo computeBufferInfoCellVertexArray = {};
            computeBufferInfoCellVertexArray.buffer                   = cellVertArrayBuffer;
            computeBufferInfoCellVertexArray.offset                   = 0;
            computeBufferInfoCellVertexArray.range = static_cast<uint32_t>(N_GRID_CELLS * 6 * sizeof(int));

            vk::WriteDescriptorSet writeComputeInfoCellVertexArray = {};
            writeComputeInfoCellVertexArray.dstSet                 = computeDescriptorSet[0];
            writeComputeInfoCellVertexArray.dstBinding             = 2;
            writeComputeInfoCellVertexArray.descriptorCount        = 1;
            writeComputeInfoCellVertexArray.dstArrayElement        = 0;
            writeComputeInfoCellVertexArray.descriptorType         = vk::DescriptorType::eStorageBuffer;
            writeComputeInfoCellVertexArray.pBufferInfo            = &computeBufferInfoCellVertexArray;

            // Set descriptor set for the cell vertex count
            vk::DescriptorBufferInfo computeBufferInfoCellVertexCount = {};
            computeBufferInfoCellVertexCount.buffer                   = cellVertCountBuffer;
            computeBufferInfoCellVertexCount.offset                   = 0;
            computeBufferInfoCellVertexCount.range = static_cast<uint32_t>(N_GRID_CELLS * sizeof(int));

            vk::WriteDescriptorSet writeComputeInfoCellVertexCount = {};
            writeComputeInfoCellVertexCount.dstSet                 = computeDescriptorSet[0];
            writeComputeInfoCellVertexCount.dstBinding             = 3;
            writeComputeInfoCellVertexCount.descriptorCount        = 1;
            writeComputeInfoCellVertexCount.dstArrayElement        = 0;
            writeComputeInfoCellVertexCount.descriptorType         = vk::DescriptorType::eStorageBuffer;
            writeComputeInfoCellVertexCount.pBufferInfo            = &computeBufferInfoCellVertexCount;

            // Set descriptor set for the buffer representing the number of vertices
            vk::DescriptorBufferInfo computeBufferInfoNumVerts = {};
            computeBufferInfoNumVerts.buffer                   = numVertsBuffer;
            computeBufferInfoNumVerts.offset                   = 0;
            computeBufferInfoNumVerts.range                    = static_cast<uint32_t>(sizeof(int));

            vk::WriteDescriptorSet writeComputeInfoNumVerts = {};
            writeComputeInfoNumVerts.dstSet                 = computeDescriptorSet[0];
            writeComputeInfoNumVerts.dstBinding             = 4;
            writeComputeInfoNumVerts.descriptorCount        = 1;
            writeComputeInfoNumVerts.dstArrayElement        = 0;
            writeComputeInfoNumVerts.descriptorType         = vk::DescriptorType::eStorageBuffer;
            writeComputeInfoNumVerts.pBufferInfo            = &computeBufferInfoNumVerts;

            // Set descriptor set for the buffer representing the sphere vertices
            vk::DescriptorBufferInfo computeBufferInfoSphereVerts = {};
            computeBufferInfoSphereVerts.buffer                   = sphereVertsBuffer;
            computeBufferInfoSphereVerts.offset                   = 0;
            computeBufferInfoSphereVerts.range = static_cast<uint32_t>(sizeof(Vertex) * sphere_verts.size());

            vk::WriteDescriptorSet writeComputeInfoSphereVerts = {};
            writeComputeInfoSphereVerts.dstSet                 = computeDescriptorSet[0];
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
            device->updateDescriptorSets(
                static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
            std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts = {computeDescriptorSetLayout};

            vk::PipelineLayoutCreateInfo computePipelineLayoutInfo = {};
            computePipelineLayoutInfo.flags                        = vk::PipelineLayoutCreateFlags();
            computePipelineLayoutInfo.setLayoutCount               = static_cast<uint32_t>(descriptorSetLayouts.size());
            computePipelineLayoutInfo.pSetLayouts                  = descriptorSetLayouts.data();
            computePipelineLayoutInfo.pushConstantRangeCount       = 0;
            computePipelineLayoutInfo.pPushConstantRanges          = 0;

            try
            {
                computePipelineLayout = device->createPipelineLayout(computePipelineLayoutInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create compute pipeline layout!");
            }

            vk::ComputePipelineCreateInfo computePipelineInfo = {};
            computePipelineInfo.flags                         = vk::PipelineCreateFlags();
            computePipelineInfo.stage                         = computeShaderStageInfo;
            computePipelineInfo.layout                        = computePipelineLayout;

            try
            {
                pipelineIdx = device->createComputePipeline(nullptr, computePipelineInfo).value;
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create compute pipeline!");
            }
        }

        VkShaderModule createShaderModule2(const std::vector<char>& code)
        {
            VkShaderModuleCreateInfo createInfo {};
            createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        }


        void cleanupSwapChain()
        {
            device->destroyImageView(depthImageView);
            device->destroyImage(depthImage);
            device->freeMemory(depthImageMemory);

            for (auto framebuffer : swapChainFramebuffers)
            {
                device->destroyFramebuffer(framebuffer);
            }

            device->freeCommandBuffers(commandPool, commandBuffers);

            device->destroyPipeline(graphicsPipeline);
            device->destroyPipelineLayout(pipelineLayout);
            device->destroyRenderPass(renderPass);

            for (auto imageView : swapChainImageViews)
            {
                device->destroyImageView(imageView);
            }

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                device->destroyBuffer(uniformUboBuffers[i]);
                device->freeMemory(uniformUboBuffersMemory[i]);
            }

            device->destroyDescriptorPool(descriptorPool);
            device->destroySwapchainKHR(swapChain);
        }

        void cleanup()
        {
            cleanupSwapChain();

            device->destroyPipeline(computePipelinePhysics);
            device->destroyPipeline(computePipelineFillCellVertex);
            device->destroyPipeline(computePipelineResetCellVertex);
            device->destroyPipeline(computePipelineSphereVertex);
            device->destroyPipelineLayout(computePipelineLayout);
            device->destroyDescriptorPool(computeDescriptorPool);
            device->destroyDescriptorSetLayout(computeDescriptorSetLayout);

            // The main texture image is used until the end of the program:
            device->destroySampler(textureSampler);
            device->destroyImageView(textureImageView);
            device->destroyImage(textureImage);
            device->freeMemory(textureImageMemory);

            device->destroyBuffer(numVertsBuffer);
            device->freeMemory(numVertsBufferMemory);

            device->destroyBuffer(vertexBuffer1);
            device->freeMemory(vertexBufferMemory1);
            device->destroyBuffer(vertexBuffer2);
            device->freeMemory(vertexBufferMemory2);

            device->destroyBuffer(indexBuffer);
            device->freeMemory(indexBufferMemory);

            device->destroyBuffer(cellVertArrayBuffer);
            device->freeMemory(cellVertArrayBufferMemory);
            device->destroyBuffer(cellVertCountBuffer);
            device->freeMemory(cellVertCountBufferMemory);
            device->destroyBuffer(sphereVertsBuffer);
            device->freeMemory(sphereVertsBufferMemory);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                device->destroySemaphore(renderFinishedSemaphores[i]);
                device->destroySemaphore(imageAvailableSemaphores[i]);
                device->destroyFence(inFlightFences[i]);
            }

            device->destroyCommandPool(commandPool);

            // surface is created by glfw, therefore not using a Unique handle
            instance->destroySurfaceKHR(surface);

            /*if (enableValidationLayers) {
                DestroyDebugUtilsMessengerEXT(*instance, callback, nullptr);
            }*/

            glfwDestroyWindow(window);

            glfwTerminate();
        }

        void recreateSwapChain()
        {
            int width = 0, height = 0;
            while (width == 0 || height == 0)
            {
                glfwGetFramebufferSize(window, &width, &height);
                glfwWaitEvents();
            }

            device->waitIdle();

            cleanupSwapChain();

            createSwapChain();
            createImageViews();
            createRenderPass();
            createGraphicsPipeline();
            createDepthResources();
            createFramebuffers();
            createuniformUboBuffers();
            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();
        }


        void loadModel()
        {
            tinyobj::attrib_t                attrib;
            std::vector<tinyobj::shape_t>    shapes;
            std::vector<tinyobj::material_t> materials;
            std::string                      warn, err;

            if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, BALL_PATH.c_str()))
            {
                std::cerr << warn + err << std::endl;
                return;
            }

            const float scale = 0.2f;

            // std::unordered_map<Vertex, uint32_t> uniqueVertices;
            for (const auto& shape : shapes)
            {
                for (const auto& index : shape.mesh.indices)
                {
                    Vertex vertex {};

                    glm::vec3 pos = {attrib.vertices[3 * index.vertex_index + 0],
                                     attrib.vertices[3 * index.vertex_index + 1],
                                     attrib.vertices[3 * index.vertex_index + 2]};

                    vertex.position   = glm::vec4(pos, 1.f) * scale;
                    vertex.position.w = 1.f;
                    vertex.color      = glm::vec4(glm::normalize(pos), 1.f);

                    model_verts.push_back(vertex);
                    model_indices.push_back(model_indices.size());
                }
            }

            std::cout << "Model vertices number: " << model_verts.size() << std::endl;
        }


        // Take a list of candidate formats in order from most desirable to least desirable,
        // and checks which is the first one that is supported
        

        

        bool hasStencilComponent(vk::Format format)
        {
            return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
        }


        /*   void createSphereVertsBuffer() {
               vk::DeviceSize bufferSize = static_cast <uint32_t>(sizeof(Vertex) * sphere_verts.size());
               // vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();;

               vk::Buffer stagingBuffer;
               vk::DeviceMemory stagingBufferMemory;
               createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible
           | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

               void* data = device->mapMemory(stagingBufferMemory, 0, bufferSize);
               memcpy(data, sphere_verts.data(), (size_t)bufferSize);
               device->unmapMemory(stagingBufferMemory);

               createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst
                   | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
                   vk::MemoryPropertyFlagBits::eDeviceLocal | vk::MemoryPropertyFlagBits::eHostVisible |
           vk::MemoryPropertyFlagBits::eHostCoherent , sphereVertsBuffer, sphereVertsBufferMemory);

               copyBuffer(stagingBuffer, sphereVertsBuffer, bufferSize);

               device->destroyBuffer(stagingBuffer);
               device->freeMemory(stagingBufferMemory);
           }*/


        void createuniformUboBuffers()
        {
            vk::DeviceSize bufferSize = static_cast<uint32_t>(sizeof(UniformBufferObject));

            uniformUboBuffers.resize(swapChainImages.size());
            uniformUboBuffersMemory.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                createBuffer(bufferSize,
                             vk::BufferUsageFlagBits::eUniformBuffer,
                             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                             uniformUboBuffers[i],
                             uniformUboBuffersMemory[i]);
            }
        }

        // Descriptor sets can't be created directly, they must be allocated from a pool like command buffers.
        // Allocate one of these descriptors for every frame.
        void createDescriptorPool()
        {
            std::array<vk::DescriptorPoolSize, 2> descriptorPoolSizes {};
            descriptorPoolSizes[0].type            = vk::DescriptorType::eUniformBuffer;
            descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            // For the allocation of the combined image sampler
            descriptorPoolSizes[1].type            = vk::DescriptorType::eCombinedImageSampler;
            descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

            vk::DescriptorPoolCreateInfo poolInfo {};
            poolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
            poolInfo.pPoolSizes    = descriptorPoolSizes.data();

            // Specify the maximum number of descriptor sets that may be allocated
            poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

            try
            {
                descriptorPool = device->createDescriptorPool(poolInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }

        void createDescriptorSets()
        {
            std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
            vk::DescriptorSetAllocateInfo        allocInfo {};
            allocInfo.descriptorPool     = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
            allocInfo.pSetLayouts        = layouts.data();

            descriptorSets.resize(swapChainImages.size());
            try
            {
                descriptorSets = device->allocateDescriptorSets(allocInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t i = 0; i < swapChainImages.size(); i++)
            {
                vk::DescriptorBufferInfo bufferInfo {};
                bufferInfo.buffer = uniformUboBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range  = sizeof(UniformBufferObject);

                // The resources for a combined image sampler structure must be specified in a vk::DescriptorImageInfo
                // struct
                vk::DescriptorImageInfo imageInfo {};
                imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                imageInfo.imageView   = textureImageView;
                imageInfo.sampler     = textureSampler;

                std::array<vk::WriteDescriptorSet, 2> descriptorWrites {};
                descriptorWrites[0].dstSet = descriptorSets[i];
                // Give our uniform buffer binding index 0
                descriptorWrites[0].dstBinding = 0;
                // Specify the first index in the array of descriptors that we want to update.
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType  = vk::DescriptorType::eUniformBuffer;
                // Specify how many array elements you want to update.
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo     = &bufferInfo;

                descriptorWrites[1].dstSet          = descriptorSets[i];
                descriptorWrites[1].dstBinding      = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo      = &imageInfo;

                device->updateDescriptorSets(
                    static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            }
        }
        

        void createCommandBuffers()
        {
            QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
            commandBuffers.resize(swapChainFramebuffers.size());

            vk::CommandBufferAllocateInfo allocInfo = {};
            allocInfo.commandPool                   = commandPool;
            allocInfo.level                         = vk::CommandBufferLevel::ePrimary;
            allocInfo.commandBufferCount            = (uint32_t)commandBuffers.size();

            try
            {
                commandBuffers = device->allocateCommandBuffers(allocInfo);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to allocate command buffers!");
            }

            for (size_t i = 0; i < commandBuffers.size(); i++)
            {
                vk::CommandBufferBeginInfo beginInfo = {};
                beginInfo.flags                      = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

                try
                {
                    commandBuffers[i].begin(beginInfo);
                }
                catch (vk::SystemError err)
                {
                    throw std::runtime_error("failed to begin recording command buffer!");
                }

                vk::RenderPassBeginInfo renderPassInfo = {};
                renderPassInfo.renderPass              = renderPass;
                renderPassInfo.framebuffer             = swapChainFramebuffers[i];
                renderPassInfo.renderArea.offset       = vk::Offset2D {0, 0};
                renderPassInfo.renderArea.extent       = swapChainExtent;

                std::array<vk::ClearValue, 2> clearValues {};
                clearValues[0].color           = std::array<float, 4> {0.0f, 0.0f, 0.0f, 1.0f};
                clearValues[1].depthStencil    = vk::ClearDepthStencilValue {1.0f, 0};
                renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
                renderPassInfo.pClearValues    = clearValues.data();

                // Bind the compute pipeline
                // vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelinePhysics);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eCompute, computePipelineResetCellVertex);

                // Bind descriptor sets for compute
                // vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0,
                // 1, &ComputeDescriptorSet, 0, nullptr);
                commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                     computePipelineLayout,
                                                     0,
                                                     1,
                                                     computeDescriptorSet.data(),
                                                     0,
                                                     nullptr);

                // Dispatch the compute kernel, with one thread for each vertex
                commandBuffers[i].dispatch(N_GRID_CELLS, 1, 1);

                vk::BufferMemoryBarrier computeToComputeBarrier = {};
                computeToComputeBarrier.srcAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier.dstAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier.srcQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier.dstQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier.buffer              = cellVertCountBuffer;
                computeToComputeBarrier.offset              = 0;
                computeToComputeBarrier.size                = N_GRID_CELLS * sizeof(int);

                vk::PipelineStageFlags computeShaderStageFlags_1(vk::PipelineStageFlagBits::eComputeShader);
                vk::PipelineStageFlags computeShaderStageFlags_2(vk::PipelineStageFlagBits::eComputeShader);
                commandBuffers[i].pipelineBarrier(computeShaderStageFlags_1,
                                                  computeShaderStageFlags_2,
                                                  vk::DependencyFlags(),
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &computeToComputeBarrier,
                                                  0,
                                                  nullptr);

                // Bind the compute pipeline
                // vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelinePhysics);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eCompute, computePipelineFillCellVertex);

                // Bind descriptor sets for compute
                // vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0,
                // 1, &ComputeDescriptorSet, 0, nullptr);
                commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                     computePipelineLayout,
                                                     0,
                                                     1,
                                                     computeDescriptorSet.data(),
                                                     0,
                                                     nullptr);

                // Dispatch the compute kernel, with one thread for each vertex
                commandBuffers[i].dispatch(uint32_t(raw_verts.size()), 1, 1);

                vk::BufferMemoryBarrier computeToComputeBarrier1 = {};
                computeToComputeBarrier1.srcAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier1.dstAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier1.srcQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier1.dstQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier1.buffer              = cellVertCountBuffer;
                computeToComputeBarrier1.offset              = 0;
                computeToComputeBarrier1.size                = N_GRID_CELLS * sizeof(int); // vertexBufferSize

                vk::PipelineStageFlags computeShaderStageFlags_3(vk::PipelineStageFlagBits::eComputeShader);
                vk::PipelineStageFlags computeShaderStageFlags_4(vk::PipelineStageFlagBits::eComputeShader);
                commandBuffers[i].pipelineBarrier(computeShaderStageFlags_3,
                                                  computeShaderStageFlags_4,
                                                  vk::DependencyFlags(),
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &computeToComputeBarrier1,
                                                  0,
                                                  nullptr);

                // Bind the compute pipeline
                // vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelinePhysics);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eCompute, computePipelinePhysics);

                // Bind descriptor sets for compute
                // vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0,
                // 1, &ComputeDescriptorSet, 0, nullptr);
                commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                     computePipelineLayout,
                                                     0,
                                                     1,
                                                     computeDescriptorSet.data(),
                                                     0,
                                                     nullptr);

                // Dispatch the compute kernel, with one thread for each vertex
                commandBuffers[i].dispatch(uint32_t(raw_verts.size()), 1, 1);

                vk::BufferMemoryBarrier computeToComputeBarrier2 = {};
                computeToComputeBarrier2.srcAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier2.dstAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToComputeBarrier2.srcQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier2.dstQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToComputeBarrier2.buffer              = vertexBuffer2;
                computeToComputeBarrier2.offset              = 0;
                computeToComputeBarrier2.size = uint32_t(raw_verts.size()) * sizeof(Vertex); // vertexBufferSize

                vk::PipelineStageFlags computeShaderStageFlags_5(vk::PipelineStageFlagBits::eComputeShader);
                vk::PipelineStageFlags computeShaderStageFlags_6(vk::PipelineStageFlagBits::eComputeShader);
                commandBuffers[i].pipelineBarrier(computeShaderStageFlags_5,
                                                  computeShaderStageFlags_6,
                                                  vk::DependencyFlags(),
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &computeToComputeBarrier2,
                                                  0,
                                                  nullptr);

                // Bind the compute pipeline
                // vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelinePhysics);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eCompute, computePipelineSphereVertex);

                // Bind descriptor sets for compute
                // vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0,
                // 1, &ComputeDescriptorSet, 0, nullptr);
                commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                                     computePipelineLayout,
                                                     0,
                                                     1,
                                                     computeDescriptorSet.data(),
                                                     0,
                                                     nullptr);

                // Dispatch the compute kernel, with one thread for each vertex
                commandBuffers[i].dispatch(uint32_t(sphere_verts.size()), 1, 1);

                // Define a memory barrier to transition the vertex buffer from a compute storage object to a vertex
                // input
                vk::BufferMemoryBarrier computeToVertexBarrier = {};
                computeToVertexBarrier.srcAccessMask =
                    vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead;
                computeToVertexBarrier.dstAccessMask       = vk::AccessFlagBits::eVertexAttributeRead;
                computeToVertexBarrier.srcQueueFamilyIndex = queueFamilyIndices.computeFamily.value();
                computeToVertexBarrier.dstQueueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
                computeToVertexBarrier.buffer              = sphereVertsBuffer;
                computeToVertexBarrier.offset              = 0;
                computeToVertexBarrier.size = uint32_t(sphere_verts.size()) * sizeof(Vertex); // vertexBufferSize

                vk::PipelineStageFlags computeShaderStageFlags(vk::PipelineStageFlagBits::eComputeShader);
                vk::PipelineStageFlags vertexShaderStageFlags(vk::PipelineStageFlagBits::eVertexInput);
                commandBuffers[i].pipelineBarrier(computeShaderStageFlags,
                                                  vertexShaderStageFlags,
                                                  vk::DependencyFlags(),
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &computeToVertexBarrier,
                                                  0,
                                                  nullptr);

                commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
                commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

                vk::Buffer     vertexBuffers[] = {sphereVertsBuffer};
                vk::DeviceSize offsets[]       = {0};
                commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);

                commandBuffers[i].bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
                commandBuffers[i].drawIndexed(static_cast<uint32_t>(uint32_t(sphere_indices.size())), 1, 0, 0, 0);

                commandBuffers[i].endRenderPass();

                try
                {
                    commandBuffers[i].end();
                }
                catch (vk::SystemError err)
                {
                    throw std::runtime_error("failed to record command buffer!");
                }
            }
        }

        void createSyncObjects()
        {
            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

            try
            {
                for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                {
                    imageAvailableSemaphores[i] = device->createSemaphore({});
                    renderFinishedSemaphores[i] = device->createSemaphore({});
                    inFlightFences[i]           = device->createFence({vk::FenceCreateFlagBits::eSignaled});
                }
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }

        void drawFrame()
        {
            device->waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

            uint32_t imageIndex;
            try
            {
                vk::ResultValue result = device->acquireNextImageKHR(
                    swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], nullptr);
                imageIndex = result.value;
            }
            catch (vk::OutOfDateKHRError err)
            {
                recreateSwapChain();
                return;
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to acquire swap chain image!");
            }

            // updateVertexBuffer(imageIndex);

            vk::DeviceSize bufferSize = static_cast<uint32_t>(raw_verts.size() * sizeof(Vertex));
            copyBuffer(vertexBuffer2, vertexBuffer1, bufferSize);
            // std::swap(descriptorSets[0], descriptorSets[1]);
            updateUniformBuffer(imageIndex);
            vk::SubmitInfo submitInfo = {};

            vk::Semaphore          waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            vk::PipelineStageFlags waitStages[]     = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
            submitInfo.waitSemaphoreCount           = 1;
            submitInfo.pWaitSemaphores              = waitSemaphores;
            submitInfo.pWaitDstStageMask            = waitStages;

            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers    = &commandBuffers[imageIndex];

            vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount  = 1;
            submitInfo.pSignalSemaphores     = signalSemaphores;

            device->resetFences(1, &inFlightFences[currentFrame]);

            try
            {
                graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to submit draw command buffer!");
            }

            vk::PresentInfoKHR presentInfo = {};
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores    = signalSemaphores;

            vk::SwapchainKHR swapChains[] = {swapChain};
            presentInfo.swapchainCount    = 1;
            presentInfo.pSwapchains       = swapChains;
            presentInfo.pImageIndices     = &imageIndex;

            vk::Result resultPresent;
            try
            {
                resultPresent = presentQueue.presentKHR(presentInfo);
            }
            catch (vk::OutOfDateKHRError err)
            {
                resultPresent = vk::Result::eErrorOutOfDateKHR;
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to present swap chain image!");
            }

            if (resultPresent == vk::Result::eSuboptimalKHR || resultPresent == vk::Result::eSuboptimalKHR ||
                framebufferResized)
            {
                framebufferResized = false;
                recreateSwapChain();
                return;
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        // Generate a new transformation every frame to make the geometry spin around.
        void updateUniformBuffer(uint32_t currentImage)
        {
            static auto startTime   = std::chrono::high_resolution_clock::now();
            auto        currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            UniformBufferObject ubo {};
            ubo.model = glm::mat4(1.f);
            ubo.view  = viewMat;
            ubo.proj  = glm::perspective(
                glm::radians(45.f), swapChainExtent.width / (float)swapChainExtent.height, 0.01f, 30.0f);
            ubo.proj[1][1] *= -1;

            void* data = device->mapMemory(uniformUboBuffersMemory[currentImage], 0, sizeof(ubo));
            memcpy(data, &ubo, sizeof(ubo));
            device->unmapMemory(uniformUboBuffersMemory[currentImage]);
        }

        void updateVertexBuffer(uint32_t currentImage)
        {
            /* void* data = device->mapMemory(vertexBufferMemory1, 0, static_cast<uint32_t>(raw_verts.size() *
             sizeof(Vertex))); memcpy(data, raw_verts.data(), sizeof(raw_verts[0]) * uint32_t(raw_verts.size()));
             device->unmapMemory(vertexBufferMemory1);*/
        }

        vk::UniqueShaderModule createShaderModule(const std::vector<char>& code)
        {
            try
            {
                return device->createShaderModuleUnique(
                    {vk::ShaderModuleCreateFlags(), code.size(), reinterpret_cast<const uint32_t*>(code.data())});
            }
            catch (vk::SystemError err)
            {
                throw std::runtime_error("failed to create shader module!");
            }
        }




        


        std::vector<const char*> getRequiredExtensions()
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

        bool checkValidationLayerSupport()
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

        static std::vector<char> readFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                std::cout << filename << std::endl;
                throw std::runtime_error("failed to open file!");
            }

            size_t            fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }
    };

}
