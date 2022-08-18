#pragma once
#include "TextureManager.h"
#include "string"
class Texture
{
	engine::DxResPtr<ID3D11ShaderResourceView> m_textureViewer;
public:
	Texture() {}
	void Load(const std::wstring& fileName, bool isSRGB = true)
	{
		m_textureViewer = engine::TextureManager::instance().LoadTexture(fileName, isSRGB);
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
};