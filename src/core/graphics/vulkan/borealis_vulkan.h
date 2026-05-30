#pragma once
#include "../../../config.h"
#include "../pipeline_config.h"

#if defined(BOREALIS_LINUX)

#include "../../types/types.h"
#include "../helpers/helpers.h"
#include "../../helpers/macros.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../helpers/texture.h"

#include <vulkan/vulkan.h>

namespace Borealis::Graphics
{
    struct BorealisVulkanRenderer : public Helpers::IBorealisRenderer
    {
    public:

        BorealisVulkanRenderer(const PipelineDesc& pipelineDesc)
            : Helpers::IBorealisRenderer(GraphicsBackend::VULKAN, pipelineDesc)
        { }

        ~BorealisVulkanRenderer() = default;

        BOREALIS_DELETE_COPY_CONSTRUCT(BorealisVulkanRenderer)
        BOREALIS_DELETE_MOVE_CONSTRUCT(BorealisVulkanRenderer)
        BOREALIS_DELETE_COPY_ASSIGN(BorealisVulkanRenderer)
        BOREALIS_DELETE_MOVE_ASSIGN(BorealisVulkanRenderer)

        Types::int64 InitializePipeline() override;
        Types::int64 DeinitializePipeline() override;

    private:

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
        bool m_isInitialized = false;
#endif
    };
}

#endif