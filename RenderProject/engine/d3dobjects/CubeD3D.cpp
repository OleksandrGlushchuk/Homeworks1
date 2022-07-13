#include "CubeD3D.h"
#include <initializer_list>

Mesh<36, MeshType::TexturedVertex3D> Cube::m_mesh = Mesh<36, MeshType::TexturedVertex3D>({
	//FRONT
	{-1, -1, -1, 0, 0}, {-1, 1, -1, 0, 1 }, {1, -1, -1, 1, 0},
	{-1, 1, -1, 0, 1}, {1, 1, -1, 1, 1}, {1, -1, -1, 1, 0},
	//BACK
	{-1, -1, 1, 1, 0}, {1, -1, 1, 0, 0}, {-1, 1, 1, 1, 1},
	{-1, 1, 1, 1, 1}, {1, -1, 1, 0, 0}, {1, 1, 1, 0, 1},
	//UP
	{-1, 1, -1, 0, 0}, {-1, 1, 1, 0, 1}, {1, 1, -1, 1, 0},
	{1, 1, -1, 1, 0}, {-1, 1, 1, 0, 1}, {1, 1, 1, 1, 1},
	//DOWN
	{-1, -1, -1, 0, 1}, {1, -1, -1, 1, 1}, {-1, -1, 1, 0, 0},
	{-1, -1, 1, 0, 0}, {1, -1, -1, 1, 1}, {1, -1, 1, 1, 0},
	//LEFT
	{-1, -1, 1, 0, 0}, {-1, 1, 1, 0, 1}, {-1, 1, -1, 1, 1},
	{-1, 1, -1, 1, 1}, {-1, -1, -1, 1, 0}, {-1, -1, 1, 0, 0},
	//RIGHT
	{1, 1, -1, 0, 1}, {1, 1, 1, 1, 1}, {1, -1, 1, 1, 0},
	{1, -1, 1, 1, 0}, {1, -1, -1, 0, 0}, {1, 1, -1, 0, 1}
	});

engine::DxResPtr<ID3D11Buffer> Cube::vertexBuffer;

Cube::Cube(float size)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = Vec3(size / 2.f, size / 2.f, size / 2.f);
	m_transform.UpdateMatrices();
	Initialize();
}

Cube::Cube(const Vec3& scale)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = scale / 2.f;
	m_transform.UpdateMatrices();
	Initialize();
}

void Cube::CreateMesh()
{
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(m_mesh.MeshSize(), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = m_mesh.GetPointerToVertices();
	HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, Cube::vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Cube::CreateShaders()
{
	engine::DxResPtr<ID3DBlob> error;

	HRESULT result = D3DCompileFromFile(L"d3dobjects/shaders/cube.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_vertexShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreateVertexShader(m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), nullptr, m_vertexShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");


	result = D3DCompileFromFile(L"d3dobjects/shaders/cube.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, m_pixelShaderBlob.reset(), error.reset());
	ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

	result = engine::s_device->CreatePixelShader(m_pixelShaderBlob->GetBufferPointer(), m_pixelShaderBlob->GetBufferSize(), nullptr, m_pixelShader.reset());
	ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
}

void Cube::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, m_mesh.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, m_mesh.TEXTURE_FORMAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 2, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
}

void Cube::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbufferDesc;
	cbufferDesc.ByteWidth = sizeof(m_transform.transform);
	cbufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	cbufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	cbufferDesc.MiscFlags = 0;
	cbufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &m_transform.transform;
	HRESULT result = engine::s_device->CreateBuffer(&cbufferDesc, &data, m_transformMatrixBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Cube::Initialize()
{
	CreateConstantBuffer();
	CreateShaders();
	CreateInputLayout();
}

bool Cube::math_intersection(math::Intersection& nearest, const ray& r) const
{
	Vec3 v0, v1, v2;
	ray rr = r;
	rr.direction.mult(m_transform.transformInv, 0);
	rr.origin.mult(m_transform.transformInv, 1);
	int k = 0;
	for (int i = 0; i < 36; i += 3)
	{
		v0 = Vec3(m_mesh[i].coord[0], m_mesh[i].coord[1], m_mesh[i].coord[2]);
		v1 = Vec3(m_mesh[i + 1].coord[0], m_mesh[i + 1].coord[1], m_mesh[i + 1].coord[2]);
		v2 = Vec3(m_mesh[i + 2].coord[0], m_mesh[i + 2].coord[1], m_mesh[i + 2].coord[2]);
		math::Triangle tr(v0, v1, v2, Vec3(Vec3::cross(v1 - v0, v2 - v0)));
		if (tr.intersection(nearest, rr))
		{
			k++;
		}
	}
	if (k > 0)
	{
		nearest.normal.mult(m_transform.transform, 0);
		nearest.normal.normalize();
		nearest.point.mult(m_transform.transform, 1);
		return true;
	}
	return false;
}

void Cube::UpdateTransformMatrixBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT result = engine::s_deviceContext->Map(m_transformMatrixBuffer.ptr(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	ALWAYS_ASSERT(result >= 0 && "Map");

	*(Matr<4>*)mappedSubresource.pData = m_transform.transform;
	engine::s_deviceContext->Unmap(m_transformMatrixBuffer.ptr(), 0);
}

void Cube::Translate(const Vec3& dv)
{
	m_transform.position += dv;
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::SetPosition(const Vec3& pos)
{
	m_transform.position = pos;
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

bool Cube::intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest)
{
	bool found = math_intersection(outNearest, _ray);
	if (found)
	{
		outRef.type = ObjRef::IntersectedType::CUBE;
		outRef.object = this;
	}
	return found;
}

void Cube::Rotate(const Quaternion& q)
{
	m_transform.rotation *= q;
	m_transform.rotation.normalize();
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
	engine::s_deviceContext->VSSetConstantBuffers(1, 1, &m_transformMatrixBuffer.ptr());
	engine::s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);

	engine::TextureManager::instance().SetSamplerState(m_samplerStateKey);
	engine::TextureManager::instance().SetTexture(m_textureKey);

	UINT stride = m_mesh.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &Cube::vertexBuffer.ptr(), &stride, &offset);
	engine::s_deviceContext->Draw(36, 0);
}



