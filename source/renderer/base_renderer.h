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

#include "main/ui/window.h"

namespace Sirion {
    namespace Renderer {
        class BaseRenderer {
        public:
        private:
            Window* window;
        };
    }
} // namespace Sirion