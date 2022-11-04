#pragma once

#include "json11.hpp"

namespace Sirion
{
    using Json = json11::Json;
    namespace Reflection
    {
        class TypeMetaRegister
        {
        public:
            static void metaRegister();
            static void metaUnregister();
        };
    } // namespace Reflection
} // namespace Sirion