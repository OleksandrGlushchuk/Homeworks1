#pragma once
#include "TextureManager.h"
#include "string"
class Texture
{
	engine::DxResPtr<ID3D11ShaderResourceView> m_textureViewer;
public:
	Texture() {}
	void Load(const std::wstring& fileName, bool need_to_force_sRGB = false)
	{
		m_textureViewer = engine::TextureManager::instance().LoadTexture(fileName, need_to_force_sRGB);
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