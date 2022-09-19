#pragma once
#include "DxRes.hpp"

namespace engine
{
	class DepthStencil
	{
		DxResPtr<ID3D11Texture2D> m_depthStencilBuffer;
		DxResPtr<ID3D11DepthStencilView> m_depthStencilView;
		DxResPtr<ID3D11DepthStencilState> m_depthStencilState;
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
		void BindDepthStencilState();
		void Clear();
		bool Is_Unused() const { return m_depthStencilView.ptr() == nullptr; }
		ID3D11DepthStencilView* GetDepthStencilViewPtr() { return m_depthStencilView.ptr(); }
		ID3D11Texture2D* GetDepthStencilResource() { return m_depthStencilBuffer.ptr(); }
	};
}