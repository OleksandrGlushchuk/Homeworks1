#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"

class Sky
{
	engine::DxResPtr<ID3D11Buffer> m_vertexBuffer;
	engine::DxResPtr<ID3D11VertexShader> m_vertexShader;
	engine::DxResPtr<ID3D11PixelShader> m_pixelShader;
	engine::DxResPtr<ID3DBlob> m_vertexShaderBlob;
	engine::DxResPtr<ID3DBlob> m_pixelShaderBlob;
	engine::DxResPtr<ID3D11InputLayout> m_inputLayout;

	Mesh<3, MeshType::Vertex2D> fullScreenTriangle;
	void CreateMesh();
	void CreateShaders();
	void CreateInputLayout();

public:
	Sky(){}
	void Initialize();
	void Draw();
};