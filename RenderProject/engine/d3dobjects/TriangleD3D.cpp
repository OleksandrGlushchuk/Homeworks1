#include "TriangleD3D.h"
void Triangle::CreateMesh()
{
	mesh[0].SetVertex(-1, -1, 1, 0, 0);
	mesh[1].SetVertex(0, 1, 0, 1, 0);
	mesh[2].SetVertex(1, -1, 0, 0, 1);


	auto vertexBufferDesc = CD3D11_BUFFER_DESC(mesh.MeshSize(), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = mesh.GetPointerToVertices();

	HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Triangle::CreateShaders()
{
	engine::DxResPtr<ID3DBlob> error;
	HRESULT result = D3DCompileFromFile(L"d3dobjects/shaders/vs_triangle.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, m_vertexBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreateVertexShader(m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), nullptr, m_vertexShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");



	result = D3DCompileFromFile(L"d3dobjects/shaders/ps_triangle.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, m_pixelBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreatePixelShader(m_pixelBlob->GetBufferPointer(), m_pixelBlob->GetBufferSize(), nullptr, m_pixelShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
}

void Triangle::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 2, m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), m_inputLayout.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
}



void Triangle::Initialize()
{
	CreateMesh();
	CreateShaders();
	CreateInputLayout();
}

void Triangle::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
	engine::s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);

	UINT stride = mesh.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer.ptr(), &stride, &offset);

	engine::s_deviceContext->Draw(3, 0);
}
