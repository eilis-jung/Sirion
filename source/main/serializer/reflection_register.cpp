#include <assert.h>

#include "reflection.h"
#include "reflection_register.h"
#include "serializer.h"

#include "_generated/reflection/all_reflection.h"
#include "_generated/serializer/all_serializer.ipp"

namespace Piccolo
{
    namespace Reflection
    {
        void TypeMetaRegister::metaUnregister() { TypeMetaRegisterinterface::unregisterAll(); }
    } // namespace Reflection
} // namespace Piccolo
