#pragma once
#include "DxRes.hpp"

namespace engine
{
	class DepthStencil
	{
		DxResPtr<ID3D11Texture2D> m_depthStencilBuffer;
		DxResPtr<ID3D11DepthStencilView> m_depthStencilView;
		D3D11_TEXTURE2D_DESC m_resourceDesc;
		void initDepthStencilResource(UINT width, UINT height, UINT sampleCount);
		void initCubemapArrayDepthStencilResource(UINT size, UINT numCubemaps);
		void initDepthStencilView();
		void initCubemapArrayDepthStencilView(UINT numCubemaps);

		void initTexture2DArrayDepthStencilResource(UINT size, UINT numTextures);
		void initTexture2DArrayDepthStencilView(UINT numTextures);
		UINT m_sampleCount;
	public:
		void Init(UINT width, UINT height, UINT sampleCount);
		void InitCubeMapArray(UINT size, UINT numCubemaps);
		void InitTexture2DArray(UINT size, UINT numTextures);
		void Resize(UINT width, UINT height);
		void Clear();
		bool Is_Unused() const { return m_depthStencilView.ptr() == nullptr; }
		const D3D11_TEXTURE2D_DESC& GetResourceDesc() const { return m_resourceDesc; }
		DxResPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return m_depthStencilView; }
		DxResPtr<ID3D11Texture2D>& GetDepthStencilResource() { return m_depthStencilBuffer; }
	};
}