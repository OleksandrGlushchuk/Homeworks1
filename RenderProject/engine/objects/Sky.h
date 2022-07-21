#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"

class Sky
{
	static engine::DxResPtr<ID3D11Buffer> s_vertexBuffer;
	static engine::DxResPtr<ID3D11VertexShader> s_vertexShader;
	static engine::DxResPtr<ID3D11PixelShader> s_pixelShader;
	static engine::DxResPtr<ID3DBlob> s_vertexShaderBlob;
	static engine::DxResPtr<ID3DBlob> s_pixelShaderBlob;

	engine::DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	static void InitTextures();
	static void InitShaders();
public:

	Sky(){}
	static void Init();
	inline void SetTexture(const std::string& _textureKey)
	{
		m_shaderResourceView.release();
		m_shaderResourceView = engine::TextureManager::instance().GetTexture(_textureKey);
	}
	void Draw();
};