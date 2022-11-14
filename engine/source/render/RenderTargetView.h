#pragma once
#include "DxRes.hpp"
static	FLOAT clearColor[4] = { 0.0f, 0.0f, 0.f, 0.f };
namespace engine
{
	class RenderTargetView
	{
		D3D11_TEXTURE2D_DESC m_renderTargetResourceDesc;
		engine::DxResPtr<ID3D11Texture2D> m_renderTargetResource;
		engine::DxResPtr<ID3D11RenderTargetView1> m_renderTargetView;
		enum class RTVResource{FLOAT16_RTR, UNORM8_RTR, RG8UNORM_RTR, SNORM16_RTR, R16UINT_RTR, EXTERNAL_RTR};
		RTVResource m_RTVResource;
		UINT m_sampleCount;
		void InitCommonResourceFields(UINT width, UINT height, UINT sampleCount)
		{
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
		}

		void InitFloat16RenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			InitCommonResourceFields(width, height, sampleCount);
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}

		void InitUnorm8RenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			InitCommonResourceFields(width, height, sampleCount);
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}

		void InitRG8UnormRenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			InitCommonResourceFields(width, height, sampleCount);
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}

		void InitSnorm16RenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			InitCommonResourceFields(width, height, sampleCount);
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_SNORM;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}

		void InitR16UintRenderTargetResource(UINT width, UINT height, UINT sampleCount)
		{
			InitCommonResourceFields(width, height, sampleCount);
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
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

		void InitSnorm16RTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::SNORM16_RTR;
			m_sampleCount = sampleCount;
			InitSnorm16RenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitUnorm8RTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::UNORM8_RTR;
			m_sampleCount = sampleCount;
			InitUnorm8RenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitRG8UnormRTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::RG8UNORM_RTR;
			m_sampleCount = sampleCount;
			InitRG8UnormRenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitR16UintRTV(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::R16UINT_RTR;
			m_sampleCount = sampleCount;
			InitR16UintRenderTargetResource(width, height, sampleCount);
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
			case RTVResource::UNORM8_RTR:
				InitUnorm8RTV(width, height, m_sampleCount);
				break;
			case RTVResource::RG8UNORM_RTR:
				InitRG8UnormRTV(width, height, m_sampleCount);
				break;
			case RTVResource::SNORM16_RTR:
				InitSnorm16RTV(width, height, m_sampleCount);
				break;
			case RTVResource::R16UINT_RTR:
				InitR16UintRTV(width, height, m_sampleCount);
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
			engine::s_deviceContext->ClearRenderTargetView(m_renderTargetView.ptr(), clearColor);
		}

		void Release()
		{
			m_renderTargetView.release();
			m_renderTargetResource.release();
			m_renderTargetResourceDesc = {};
		}
		const D3D11_TEXTURE2D_DESC& GetResourceDesc() const { return m_renderTargetResourceDesc; }
		DxResPtr<ID3D11Texture2D>& GetResource() { return m_renderTargetResource; }
		DxResPtr<ID3D11RenderTargetView1>& GetRTV() { return m_renderTargetView; }
		ID3D11RenderTargetView* const * GetRTVPtrToPrt() { return (ID3D11RenderTargetView * const*)&m_renderTargetView.ptr(); }
		UINT GetWidth() const {	return m_renderTargetResourceDesc.Width;}
		UINT GetHeight() const { return m_renderTargetResourceDesc.Height;}
	};
}