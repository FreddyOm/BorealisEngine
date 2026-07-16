#pragma once
#include "../../config.h"
#include "helpers/helpers.h"
#include "../memory/ref_cnt_auto_ptr.h"


// Platform specific includes
#if defined(BOREALIS_WIN)

// TODO: Differentiate rendering backends
// TODO: Also create common interface and bnackend independence layer
// TODO: Consider using d3d11on12 https://github.com/microsoft/D3D11On12/tree/master

#include "d3d11/borealis_d3d11.h"
#include "d3d12/borealis_d3d12.h"
#include "vulkan/borealis_vulkan.h"


#elif defined(BOREALIS_LINUX) || defined(BOREALIS_OSX)
// TODO: Implement
#include "vulkan/borealis_vulkan.h"

#endif

namespace Borealis::Graphics
{
    class BOREALIS_API RendererLocator
    {
    public:
        static void Initialize() 
        { 
            Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::CORESYS);

            m_NullService = Memory::RefCntAutoPtr<Helpers::NullRenderer>::Allocate();
        }

        static Memory::RefCntAutoPtr<Helpers::IBorealisRenderer>& Get() { return m_Service; }

        static void Provide(Memory::RefCntAutoPtr<Helpers::IBorealisRenderer> service)
        {
            if (!service.IsValid())
            {
                // Revert to null service.
                m_Service = Memory::RefCntAutoPtr<Helpers::NullRenderer>::DynamicCastTo<Helpers::IBorealisRenderer>(m_NullService);
            }
            else
            {
                m_Service = service;
            }
        }

        static void Reset()
        {
            if (m_Service.IsValid())
                m_Service.Reset();

            if (m_NullService.IsValid())
                m_NullService.Reset();
        }

    private:
        static Memory::RefCntAutoPtr<Helpers::IBorealisRenderer> m_Service;
        static Memory::RefCntAutoPtr<Helpers::NullRenderer> m_NullService;
    };
}