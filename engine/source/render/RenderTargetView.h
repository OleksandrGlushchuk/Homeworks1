#pragma once
#include "DxRes.hpp"
#include <functional>
namespace engine
{
	class RenderTargetView
	{
		D3D11_TEXTURE2D_DESC m_renderTargetResourceDesc;
		engine::DxResPtr<ID3D11Texture2D> m_renderTargetResource;
		engine::DxResPtr<ID3D11RenderTargetView1> m_renderTargetView;
		enum class RTVResource{FLOAT16_RTR, DEPTH_RTR, EXTERNAL_RTR};
		RTVResource m_RTVResource;
		UINT m_sampleCount;

		void InitFloat16RenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
			m_renderTargetResourceDesc.Width = std::max<UINT>(width, 8u);
			m_renderTargetResourceDesc.Height = std::max<UINT>(height, 8u);
			m_renderTargetResourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			m_renderTargetResourceDesc.ArraySize = 1;
			m_renderTargetResourceDesc.MipLevels = 1;
			m_renderTargetResourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			m_renderTargetResourceDesc.CPUAccessFlags = 0;
			m_renderTargetResourceDesc.SampleDesc.Count = sampleCount;
			m_renderTargetResourceDesc.SampleDesc.Quality = 0;
			m_renderTargetResourceDesc.MiscFlags = 0;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}
		void InitDepthRenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			m_renderTargetResourceDesc.Width = std::max<UINT>(width, 8u);
			m_renderTargetResourceDesc.Height = std::max<UINT>(height, 8u);
			m_renderTargetResourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			m_renderTargetResourceDesc.ArraySize = 1;
			m_renderTargetResourceDesc.MipLevels = 1;
			m_renderTargetResourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			m_renderTargetResourceDesc.CPUAccessFlags = 0;
			m_renderTargetResourceDesc.SampleDesc.Count = sampleCount;
			m_renderTargetResourceDesc.SampleDesc.Quality = 0;
			m_renderTargetResourceDesc.MiscFlags = 0;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}
	public:
		RenderTargetView() {}

		void Init(engine::DxResPtr<ID3D11Texture2D>& renderTargetResource)
		{
			m_renderTargetResource = renderTargetResource;
			m_renderTargetResource->GetDesc(&m_renderTargetResourceDesc);
			m_RTVResource = RTVResource::EXTERNAL_RTR;
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitFloat16RTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::FLOAT16_RTR;
			m_sampleCount = sampleCount;
			InitFloat16RenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitDepthRTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::DEPTH_RTR;
			m_sampleCount = sampleCount;
			InitDepthRenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void Resize(UINT width, UINT height)
		{
			switch (m_RTVResource)
			{
			case RTVResource::FLOAT16_RTR :
				InitFloat16RTV(width, height, m_sampleCount);
				break;
			case RTVResource::EXTERNAL_RTR :
				DebugBreak();
				std::abort();
				break;
			default:
				DebugBreak();
				std::abort();
				break;
			}
		}
		
		void Clear()
		{
			FLOAT color[4] = { 0.25f, 0.5f, 1.f, 1.f };
			engine::s_deviceContext->ClearRenderTargetView(m_renderTargetView.ptr(), color);
		}

		void Release()
		{
			m_renderTargetView.release();
			m_renderTargetResource.release();
			m_renderTargetResourceDesc = {};
		}
		ID3D11Texture2D* GetResourcePtr() { return m_renderTargetResource.ptr(); }
		ID3D11RenderTargetView1* GetRTVPtr() { return m_renderTargetView.ptr(); }
		ID3D11RenderTargetView* const * GetRTVPtrToPrt() { return (ID3D11RenderTargetView * const*)&m_renderTargetView.ptr(); }
		UINT GetWidth() const {	return m_renderTargetResourceDesc.Width;}
		UINT GetHeight() const { return m_renderTargetResourceDesc.Height;}
	};
}