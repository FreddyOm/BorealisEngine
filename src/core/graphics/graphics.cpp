#include "graphics.h"

namespace Borealis::Graphics
{
    // Service locator global static data
    Memory::RefCntAutoPtr<Helpers::IBorealisRenderer> RendererLocator::m_Service;
    Memory::RefCntAutoPtr<Helpers::NullRenderer> RendererLocator::m_NullService;
}    // namespace Borealis::Graphics