#include "borealis_vulkan.h"


#if defined(BOREALIS_LINUX)


namespace Borealis::Graphics
{

    Types::int64 BorealisVulkanRenderer::InitializePipeline()
    {
        
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		m_isInitialized = false;

#endif
        return 0;
    }

    Types::int64 BorealisVulkanRenderer::DeinitializePipeline()
    {

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		Assert(m_isInitialized, "Trying to deinitialize Vulkan pipeline that hasn't been initialized!");
#endif

        return 0;
    }

    void Borealis::BorealisVulkanRenderer::WaitForPendingOperations()
    {
        return;
    }
}

#endif