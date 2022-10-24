#pragma once
#include "singletones/TextureManager.h"
#include "string"
namespace engine
{
	class Texture
	{
		DxResPtr<ID3D11ShaderResourceView> m_textureViewer;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_SRVDesc;
		DxResPtr<ID3D11Texture2D> m_resource;
		D3D11_TEXTURE2D_DESC m_resourceDesc;

	public:
		Texture() {}
		void PointToResource(DxResPtr<ID3D11Texture2D> &texture, const D3D11_TEXTURE2D_DESC& desc)
		{
			HRESULT result;
			if (desc.Format != DXGI_FORMAT_R24G8_TYPELESS)
				m_SRVDesc.Format = desc.Format;
			else
				m_SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;


			if (desc.SampleDesc.Count == 1)
			{
				m_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
				m_SRVDesc.Texture2D.MipLevels = desc.MipLevels;
				m_SRVDesc.Texture2D.MostDetailedMip = 0;
			}
			else
			{
				m_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMS;
			}
			result = engine::s_device->CreateShaderResourceView(texture.ptr(), &m_SRVDesc, m_textureViewer.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");
			m_resource = texture;
			m_resource->GetDesc(&m_resourceDesc);

		}
		void Load(const std::wstring& fileName, bool need_to_force_sRGB = false)
		{
			m_textureViewer = engine::TextureManager::instance().LoadTexture(fileName, m_resource, need_to_force_sRGB);
			m_textureViewer->GetDesc(&m_SRVDesc);
			m_resource->GetDesc(&m_resourceDesc);
		}
		void Bind(UINT startSlot = 0)
		{
			engine::s_deviceContext->PSSetShaderResources(startSlot, 1, &m_textureViewer.ptr());
		}
		bool operator==(const Texture& tex) const
		{
			return m_textureViewer == tex.m_textureViewer;
		}
		bool operator!=(const Texture& tex) const
		{
			return m_textureViewer != tex.m_textureViewer;
		}
		const D3D11_SHADER_RESOURCE_VIEW_DESC& GetDesc() const { return m_SRVDesc; }

		DxResPtr<ID3D11Texture2D>& GetResource()
		{
			return m_resource;
		}

		D3D11_TEXTURE2D_DESC& GetResourceDesc()
		{
			return m_resourceDesc;
		}

		void CreateCopy(const D3D11_TEXTURE2D_DESC& desc)
		{
			HRESULT result = engine::s_device->CreateTexture2D(&desc, nullptr, m_resource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
			PointToResource(m_resource, desc);
		}

		void Copy(DxResPtr<ID3D11Texture2D>& srcResource)
		{
			engine::s_deviceContext->CopyResource(m_resource.ptr(), srcResource.ptr());
		}
	};
}