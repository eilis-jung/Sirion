#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace Sirion
{
    using Vector3 = glm::vec3;
    using Vector4 = glm::vec4;
    using Matrix4 = glm::mat4;

    static const float Math_POS_INFINITY = std::numeric_limits<float>::infinity();
    static const float Math_NEG_INFINITY = -std::numeric_limits<float>::infinity();
    static const float Math_PI           = 3.14159265358979323846264338327950288f;
    static const float Math_ONE_OVER_PI  = 1.0f / Math_PI;
    static const float Math_TWO_PI       = 2.0f * Math_PI;
    static const float Math_HALF_PI      = 0.5f * Math_PI;
    static const float Math_fDeg2Rad     = Math_PI / 180.0f;
    static const float Math_fRad2Deg     = 180.0f / Math_PI;
    static const float Math_LOG2         = log(2.0f);
    static const float Math_EPSILON      = 1e-6f;

    static const float Float_EPSILON  = FLT_EPSILON;
    static const float Double_EPSILON = DBL_EPSILON;

}