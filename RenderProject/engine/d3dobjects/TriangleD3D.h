#pragma once
#include "../render/DxRes.hpp"
#include "VertexMesh.h"
class Triangle
{
	engine::DxResPtr<ID3D11Buffer> m_vertexBuffer;
	engine::DxResPtr<ID3D11VertexShader> m_vertexShader;
	engine::DxResPtr<ID3D11PixelShader> m_pixelShader;
	engine::DxResPtr<ID3DBlob> m_vertexBlob;
	engine::DxResPtr<ID3DBlob> m_pixelBlob;
	engine::DxResPtr<ID3D11InputLayout> m_inputLayout;

	VertexMesh<3> mesh;

	void CreateMesh();
	void CreateShaders();
	void CreateInputLayout();
public:
	Triangle() {};
	void Initialize();
	void Draw();
};