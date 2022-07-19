#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"

class Sky
{
	static engine::DxResPtr<ID3D11InputLayout> s_inputLayout;
	static Mesh<3, MeshType::Vertex2D> fullScreenTriangle;

	engine::DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;

public:
	static engine::DxResPtr<ID3D11Buffer> m_vertexBuffer;
	static engine::DxResPtr<ID3D11VertexShader> s_vertexShader;
	static engine::DxResPtr<ID3D11PixelShader> s_pixelShader;
	static engine::DxResPtr<ID3DBlob> s_vertexShaderBlob;
	static engine::DxResPtr<ID3DBlob> s_pixelShaderBlob;

	Sky(){}
	static void CreateVertexBuffer();
	static void CreateInputLayout();

	inline void SetTexture(const std::string& _textureKey)
	{
		m_shaderResourceView.release();
		m_shaderResourceView = engine::TextureManager::instance().GetTexture(_textureKey);
	}
	void Draw();
};