#include "TriangleD3D.h"

void Triangle::CreateMesh()
{
	mesh[0].SetVertex(-1, -1, 2.1f, 0, 0);
	mesh[1].SetVertex(0, 1, 2.1f, 0.5f, 1);
	mesh[2].SetVertex(1, -1, 2.1f, 1, 0);

	auto vertexBufferDesc = CD3D11_BUFFER_DESC(mesh.MeshSize(), D3D11_BIND_VERTEX_BUFFER);

	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = mesh.GetPointerToVertices();

	HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Triangle::CreateShaders()
{
	engine::DxResPtr<ID3DBlob> error;

	HRESULT result = D3DCompileFromFile(L"d3dobjects/shaders/triangle.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_vertexShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreateVertexShader(m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), nullptr, m_vertexShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");

	result = D3DCompileFromFile(L"d3dobjects/shaders/triangle.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_pixelShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreatePixelShader(m_pixelShaderBlob->GetBufferPointer(), m_pixelShaderBlob->GetBufferSize(), nullptr, m_pixelShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
}

void Triangle::CreateInputLayout()
{
	/////----HINT----//////
	/*typedef struct D3D11_INPUT_ELEMENT_DESC
	{
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	} 	D3D11_INPUT_ELEMENT_DESC;*/

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, mesh.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, mesh.TEXTURE_FORMAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 2, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
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

	engine::TextureManager::instance().SetSamplerState("ss0");
	engine::TextureManager::instance().SetTexture("brick");


	UINT stride = mesh.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer.ptr(), &stride, &offset);

	engine::s_deviceContext->Draw(3, 0);
}
