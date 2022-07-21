#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"
#include "Cube.h"

template<typename meshType=MeshType::TexturedVertex3D>
class Triangle
{
	engine::DxResPtr<ID3D11Buffer> m_vertexBuffer;
	engine::DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	engine::DxResPtr<ID3D11InputLayout> s_inputLayout;

	static engine::DxResPtr<ID3D11VertexShader> s_vertexShader;
	static engine::DxResPtr<ID3D11PixelShader> s_pixelShader;
	static engine::DxResPtr<ID3DBlob> s_vertexShaderBlob;
	static engine::DxResPtr<ID3DBlob> s_pixelShaderBlob;

	Mesh<3, meshType> m_mesh;

	void CreateVertexBuffer();
	static void InitShaders();
	static void InitTextures();
public:
	static void Init();
	Triangle() {};
	Triangle(const std::initializer_list<meshType>& init)
	{
		m_mesh = Mesh<3, meshType>(init);
		CreateVertexBuffer();
		CreateInputLayout();
	}

	void CreateInputLayout();
	inline void SetTexture(const std::string& _textureKey)
	{
		m_shaderResourceView.release();
		m_shaderResourceView = engine::TextureManager::instance().GetTexture(_textureKey);
	}
	void Draw();
};
template<typename meshType>
engine::DxResPtr<ID3D11VertexShader> Triangle<meshType>::s_vertexShader;

template<typename meshType>
engine::DxResPtr<ID3D11PixelShader> Triangle<meshType>::s_pixelShader;

template<typename meshType>
engine::DxResPtr<ID3DBlob> Triangle<meshType>::s_vertexShaderBlob;

template<typename meshType>
engine::DxResPtr<ID3DBlob> Triangle<meshType>::s_pixelShaderBlob;

template<typename meshType>
inline void Triangle<meshType>::CreateVertexBuffer()
{
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(m_mesh.MeshSize(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = m_mesh.GetPointerToVertices();
	HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

template<>
inline void Triangle<MeshType::TexturedVertex3D>::InitShaders()
{
	engine::ShaderManager::instance().InitShaders(L"source/shaders/triangle.hlsl", "triangle");
	engine::ShaderManager::instance().GetShaderBlobs("triangle", Triangle<MeshType::TexturedVertex3D>::s_vertexShaderBlob, Triangle<MeshType::TexturedVertex3D>::s_pixelShaderBlob);
	engine::ShaderManager::instance().GetShaders("triangle", Triangle<MeshType::TexturedVertex3D>::s_vertexShader, Triangle<MeshType::TexturedVertex3D>::s_pixelShader);
}

template<typename meshType>
inline void Triangle<meshType>::InitTextures()
{
	Cube::InitTextures();
}

template<typename meshType>
inline void Triangle<meshType>::Init()
{
	InitTextures();
	InitShaders();
}

template<>
inline void Triangle<MeshType::TexturedVertex3D>::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, m_mesh.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, m_mesh.TEXTURE_FORMAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 2, s_vertexShaderBlob->GetBufferPointer(), s_vertexShaderBlob->GetBufferSize(), s_inputLayout.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
}

template<typename meshType>
inline void Triangle<meshType>::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(s_inputLayout.ptr());
	engine::s_deviceContext->VSSetShader(s_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(s_pixelShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

	UINT stride = m_mesh.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer.ptr(), &stride, &offset);

	engine::s_deviceContext->Draw(3, 0);
}

