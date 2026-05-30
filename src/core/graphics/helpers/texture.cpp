#include "texture.h"

#ifdef BOREALIS_WIN

#include "../d3d12/borealis_d3d12.h"

#else

#endif


namespace Borealis::Graphics
{

#ifdef BOREALIS_WIN

	const Types::uint16 Borealis::Graphics::BorealisD3D12Texture::GetWidth() const
	{
		return m_Width;
	}

	BorealisD3D12Texture::~BorealisD3D12Texture()
	{
		g_SRVDescHeapAllocator.Free(m_CPUHandle, m_GPUHandle);
		m_TextureResource->Release();
	}

	const Types::uint16 Borealis::Graphics::BorealisD3D12Texture::GetHeight() const
	{
		return m_Height;
	}

	void BorealisD3D12Texture::CommitTexture()
	{
		m_Height = m_TextureResource->GetDesc().Height;
		m_Width = m_TextureResource->GetDesc().Width;
	}

#else

	const Types::uint16 Borealis::Graphics::BorealisVulkanTexture::GetWidth() const
	{
		return m_Width;
	}

	const Types::uint16 Borealis::Graphics::BorealisVulkanTexture::GetHeight() const
	{
		return m_Height;
	}

#endif
}