#include "helpers.h"

namespace Borealis::Graphics::Helpers
{
	
}


namespace Borealis::Graphics
{
	const Types::uint16 Borealis::Graphics::BorealisD3D12Texture::GetWidth() const
	{
		return m_Width;
	}

	const Types::uint16 Borealis::Graphics::BorealisD3D12Texture::GetHeight() const
	{
		return m_Height;
	}

	const Types::uint16 Borealis::Graphics::BorealisVulkanTexture::GetWidth() const
	{
		return m_Width;
	}

	const Types::uint16 Borealis::Graphics::BorealisVulkanTexture::GetHeight() const
	{
		return m_Height;
	}
}