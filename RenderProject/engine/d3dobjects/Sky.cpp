#include "Sky.h"

void Sky::CreateMesh()
{
	fullScreenTriangle[0].SetVertex(-1, -1);
	fullScreenTriangle[1].SetVertex(-1, 3);
	fullScreenTriangle[2].SetVertex(3, -1);

	auto bufferDesc = CD3D11_BUFFER_DESC(fullScreenTriangle.MeshSize(), D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = fullScreenTriangle.GetPointerToVertices();
	HRESULT result = engine::s_device->CreateBuffer(&bufferDesc, &vertexData , m_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Sky::CreateShaders()
{
	engine::DxResPtr<ID3DBlob> error;

	HRESULT result = D3DCompileFromFile(L"d3dobjects/shaders/sky.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_vertexShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreateVertexShader(m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), nullptr, m_vertexShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");


	result = D3DCompileFromFile(L"d3dobjects/shaders/sky.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_pixelShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreatePixelShader(m_pixelShaderBlob->GetBufferPointer(), m_pixelShaderBlob->GetBufferSize(), nullptr, m_pixelShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
}

void Sky::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, fullScreenTriangle.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 1, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
}

void Sky::Initialize()
{
	CreateMesh();
	CreateShaders();
	CreateInputLayout();
}

void Sky::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
	engine::s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);

	engine::TextureManager::instance().SetSamplerState(engine::TextureManager::instance().GetGlobalSamplerStateKey());
	engine::TextureManager::instance().SetTexture("sky");


	UINT stride = fullScreenTriangle.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer.ptr(), &stride, &offset);

	engine::s_deviceContext->Draw(3, 0);
}
