#pragma once
#include "TextureManager.h"
#include "string"
class Texture
{
	engine::DxResPtr<ID3D11ShaderResourceView> m_textureViewer;
public:
	Texture() {}
	void Load(const std::wstring& fileName)
	{
		m_textureViewer = engine::TextureManager::instance().LoadTexture(fileName);
	}
	void Bind()
	{
		engine::s_deviceContext->PSSetShaderResources(0, 1, &m_textureViewer.ptr());
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