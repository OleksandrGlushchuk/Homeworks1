#include "Sky.h"
engine::DxResPtr<ID3D11Buffer> Sky::m_vertexBuffer;
engine::DxResPtr<ID3D11VertexShader> Sky::s_vertexShader;
engine::DxResPtr<ID3D11PixelShader> Sky::s_pixelShader;
engine::DxResPtr<ID3DBlob> Sky::s_vertexShaderBlob;
engine::DxResPtr<ID3DBlob> Sky::s_pixelShaderBlob;
engine::DxResPtr<ID3D11InputLayout> Sky::s_inputLayout;
Mesh<3, MeshType::Vertex2D> Sky::fullScreenTriangle = Mesh<3, MeshType::Vertex2D>({
	{-1,-1}, {-1,3}, {3,-1}
	});

void Sky::CreateVertexBuffer()
{
	auto bufferDesc = CD3D11_BUFFER_DESC(fullScreenTriangle.MeshSize(), D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = fullScreenTriangle.GetPointerToVertices();
	HRESULT result = engine::s_device->CreateBuffer(&bufferDesc, &vertexData , m_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Sky::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, fullScreenTriangle.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 1, s_vertexShaderBlob->GetBufferPointer(), s_vertexShaderBlob->GetBufferSize(), s_inputLayout.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
}

void Sky::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(s_inputLayout.ptr());
	engine::s_deviceContext->VSSetShader(s_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(s_pixelShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

	UINT stride = fullScreenTriangle.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer.ptr(), &stride, &offset);

	engine::s_deviceContext->Draw(3, 0);
}
