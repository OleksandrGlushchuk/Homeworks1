#pragma once
#include "DxRes.hpp"
#include "DepthStencil.h"
#include <functional>
namespace engine
{
	class RenderTarget
	{
		D3D11_TEXTURE2D_DESC m_renderTargetResourceDesc;
		DepthStencil m_depthStencil;
		engine::DxResPtr<ID3D11Texture2D> m_renderTargetResource;
		engine::DxResPtr<ID3D11RenderTargetView1> m_renderTargetView;
		enum class RTVResource{FLOAT16_RTR, CUBEMAP_ARRAY_RTR, EXTERNAL_RTR};
		RTVResource m_RTVResource;
		UINT m_sampleCount;
		void InitCubeMapArrayRenderTargetResource(UINT size, UINT numCubemaps)
		{
			m_renderTargetResourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			m_renderTargetResourceDesc.Width = std::max<UINT>(size, 8u);
			m_renderTargetResourceDesc.Height = m_renderTargetResourceDesc.Width;
			m_renderTargetResourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			m_renderTargetResourceDesc.ArraySize = numCubemaps * 6;
			m_renderTargetResourceDesc.MipLevels = 1;
			m_renderTargetResourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			m_renderTargetResourceDesc.CPUAccessFlags = 0;
			m_renderTargetResourceDesc.SampleDesc.Count = 1;
			m_renderTargetResourceDesc.SampleDesc.Quality = 0;
			m_renderTargetResourceDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
			HRESULT result = engine::s_device->CreateTexture2D(&m_renderTargetResourceDesc, nullptr, m_renderTargetResource.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
		}

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
	public:
		RenderTarget() {}

		void InitRenderTargetView(engine::DxResPtr<ID3D11Texture2D>& renderTargetResource)
		{
			m_renderTargetResource = renderTargetResource;
			m_renderTargetResource->GetDesc(&m_renderTargetResourceDesc);
			m_RTVResource = RTVResource::EXTERNAL_RTR;
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitCubeMapArrayRenderTargetView(UINT size, UINT numCubemaps)
		{
			m_RTVResource = RTVResource::CUBEMAP_ARRAY_RTR;
			m_sampleCount = 1;
			InitCubeMapArrayRenderTargetResource(size, numCubemaps);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitCubeMapArrayDepthStencil(UINT size, UINT numCubemaps)
		{
			m_depthStencil.InitCubeMapArray(size, numCubemaps);
		}

		void InitFloat16RenderTargetView(UINT width, UINT height, UINT sampleCount)
		{
			m_RTVResource = RTVResource::FLOAT16_RTR;
			m_sampleCount = sampleCount;
			InitFloat16RenderTargetResource(width, height, sampleCount);
			HRESULT result = engine::s_device->CreateRenderTargetView1(m_renderTargetResource.ptr(), nullptr, m_renderTargetView.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
		}

		void InitDepthStencil(UINT width, UINT height, UINT sampleCount)
		{
			m_depthStencil.Init(width, height, sampleCount);
		}

		void ResizeRenderTargetView(UINT width, UINT height)
		{
			switch (m_RTVResource)
			{
			case RTVResource::FLOAT16_RTR :
				InitFloat16RenderTargetView(width, height, m_sampleCount);
				break;
			case RTVResource::EXTERNAL_RTR :
			case RTVResource::CUBEMAP_ARRAY_RTR:
				DebugBreak();
				std::abort();
				break;
			default:
				DebugBreak();
				std::abort();
				break;
			}
		}
		void ResizeDepthStencil(UINT width, UINT height)
		{
			m_depthStencil.Resize(width, height);
		}
		
		void ClearRendetTargetView()
		{
			FLOAT color[4] = { 0.25f, 0.5f, 1.f, 1.f };
			engine::s_deviceContext->ClearRenderTargetView(m_renderTargetView.ptr(), color);
		}
		void ClearDepthStencil()
		{
			m_depthStencil.Clear();
		}
		void Bind(DepthStencil otherDepthStencil = {})
		{
			if (otherDepthStencil.Is_Unused())
			{
				if (!m_depthStencil.Is_Unused())
				{
					m_depthStencil.BindDepthStencilState();
					engine::s_deviceContext->OMSetRenderTargets(1, m_renderTargetView.ptr() == nullptr ? nullptr : 
						(ID3D11RenderTargetView* const*)&m_renderTargetView.ptr(), m_depthStencil.GetDepthStencilViewPtr());
				}
				else
					engine::s_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)&m_renderTargetView.ptr(), nullptr);
			}
			else
			{
				otherDepthStencil.Clear();
				otherDepthStencil.BindDepthStencilState();
				engine::s_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)&m_renderTargetView.ptr(), otherDepthStencil.GetDepthStencilViewPtr());
			}
		}
		void ReleaseRenderTarget()
		{
			m_renderTargetView.release();
			m_renderTargetResource.release();
			m_renderTargetResourceDesc = {};
		}
		ID3D11Texture2D* GetRenderTergetResource() { return m_renderTargetResource.ptr(); }
		UINT GetWidth() const {	return m_renderTargetResourceDesc.Width;}
		UINT GetHeight() const { return m_renderTargetResourceDesc.Height;}
		DepthStencil& GetDepthStencil()
		{
			return m_depthStencil;
		}
	};
}