#pragma once
#include "../../../config.h"
#include "../../helpers/macros.h"
#include "../pipeline_config.h"
#include "../../types/types.h"
#include "../../math/math.h"

#ifdef BOREALIS_WIN
#include <d3d12.h>
#endif

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		IBorealisRenderer(Borealis::Graphics::GraphicsBackend backend, const Borealis::Graphics::PipelineDesc& pipelineDesc)
			: m_GraphicsBackend(backend), m_PipelineDesc(pipelineDesc)
		{ }

		virtual ~IBorealisRenderer() = default;
		
        BOREALIS_DELETE_COPY_CONSTRUCT(IBorealisRenderer)
        BOREALIS_DELETE_MOVE_CONSTRUCT(IBorealisRenderer)
        BOREALIS_DELETE_COPY_ASSIGN(IBorealisRenderer)
        BOREALIS_DELETE_MOVE_ASSIGN(IBorealisRenderer)

		virtual Borealis::Types::int64 InitializePipeline() = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
		virtual void WaitForPendingOperations() = 0;
		virtual void StartFrame(Math::Vector4<float> clearColor = Math::Vector4<float>{ 0.1, 0.3, 0.5, 1.0 }) = 0;
		virtual HRESULT PresentFrame() = 0;
		virtual const bool IsVsyncEnabled() const = 0;
		virtual void SetVsyncEnabled(const bool enabled) = 0;
		
		const Borealis::Graphics::PipelineDesc m_PipelineDesc;
		const Borealis::Graphics::GraphicsBackend m_GraphicsBackend;
	};


	// Null implementation for service locator fallback 
	class BOREALIS_API NullRenderer : public IBorealisRenderer
	{
	public:
		NullRenderer()
			: IBorealisRenderer(Borealis::Graphics::GraphicsBackend::UNDEFINED, Borealis::Graphics::PipelineDesc())
		{ }
		virtual ~NullRenderer() { }

		virtual Borealis::Types::int64 InitializePipeline() override { return 0; }
		virtual Borealis::Types::int64 DeinitializePipeline() override { return 0; }
		virtual void WaitForPendingOperations() override { }
		virtual void StartFrame(Math::Vector4<float> clearColor) override { }
		virtual HRESULT PresentFrame() override { return S_OK; }
		virtual const bool IsVsyncEnabled() const override { return false; }
		virtual void SetVsyncEnabled(const bool enabled) override { }
	};
}
