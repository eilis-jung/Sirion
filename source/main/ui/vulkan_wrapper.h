#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
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

#include "math/math.h"
#include "renderer/utils/renderer_config.h"

#include "shader/generated/include/shader_vert.h"
#include "shader/generated/include/shader_frag.h"
#include "shader/generated/include/fillCellVertexInfo_comp.h"
#include "shader/generated/include/physicsCompute_comp.h"
#include "shader/generated/include/resetCellVertexInfo_comp.h"
#include "shader/generated/include/sphereVertexCompute_comp.h"

namespace Sirion
{
    class VulkanWrapper
    {
    protected:
        RendererConfig m_RendererConfig;
    
    public:
        VulkanWrapper() { 
            std::cout << "test" << std::endl;
        }


    };

}

