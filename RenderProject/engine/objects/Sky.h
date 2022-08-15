#pragma once
#include "../render/globals.hpp"

class Sky
{
	engine::DxResPtr<ID3D11VertexShader> s_vertexShader;
	engine::DxResPtr<ID3D11PixelShader> s_pixelShader;
	engine::DxResPtr<ID3DBlob> s_vertexShaderBlob;
	engine::DxResPtr<ID3DBlob> s_pixelShaderBlob;

	engine::DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	void InitTextures();
	void InitShaders();
public:

	Sky(){}
	void Init();
	inline void SetTexture(const std::wstring& _textureKey)
	{
		m_shaderResourceView.release();
		m_shaderResourceView = engine::TextureManager::instance().GetTexture(_textureKey);
	}
	void Draw();
};