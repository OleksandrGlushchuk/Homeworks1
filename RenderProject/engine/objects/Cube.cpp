#include "Cube.h"
#include <initializer_list>

engine::DxResPtr<ID3D11InputLayout> Cube::s_inputLayout;
engine::DxResPtr<ID3D11Buffer> Cube::s_vertexBuffer;

engine::DxResPtr<ID3D11VertexShader> Cube::s_vertexShader;
engine::DxResPtr<ID3D11PixelShader> Cube::s_pixelShader;
engine::DxResPtr<ID3DBlob> Cube::s_vertexShaderBlob;
engine::DxResPtr<ID3DBlob> Cube::s_pixelShaderBlob;

Mesh<36, MeshType::TexturedVertex3D> Cube::s_mesh = Mesh<36, MeshType::TexturedVertex3D>({
	//FRONT
	{-0.5f, -0.5f, -0.5f, 0, 1}, {-0.5f, 0.5f, -0.5f, 0, 0 }, {0.5f, -0.5f, -0.5f, 1, 1},
	{-0.5f, 0.5f, -0.5f, 0, 0}, {0.5f, 0.5f, -0.5f, 1, 0}, {0.5f, -0.5f, -0.5f, 1, 1},
	//BACK
	{-0.5f, -0.5f, 0.5f, 1, 1}, {0.5f, -0.5f, 0.5f, 0, 1}, {-0.5f, 0.5f, 0.5f, 1, 0},
	{-0.5f, 0.5f, 0.5f, 1, 0}, {0.5f, -0.5f, 0.5f, 0, 1}, {0.5f, 0.5f, 0.5f, 0, 0},
	//UP
	{-0.5f, 0.5f, -0.5f, 0, 1}, {-0.5f, 0.5f, 0.5f, 0, 0}, {0.5f, 0.5f, -0.5f, 1, 1},
	{0.5f, 0.5f, -0.5f, 1, 1}, {-0.5f, 0.5f, 0.5f, 0, 0}, {0.5f, 0.5f, 0.5f, 1, 0},
	//DOWN
	{-0.5f, -0.5f, -0.5f, 0, 0}, {0.5f, -0.5f, -0.5f, 1, 0}, {-0.5f, -0.5f, 0.5f, 0, 1},
	{-0.5f, -0.5f, 0.5f, 0, 1}, {0.5f, -0.5f, -0.5f, 1, 0}, {0.5f, -0.5f, 0.5f, 1, 1},
	//LEFT
	{-0.5f, -0.5f, 0.5f, 0, 1}, {-0.5f, 0.5f, 0.5f, 0, 0}, {-0.5f, 0.5f, -0.5f, 1, 0},
	{-0.5f, 0.5f, -0.5f, 1, 0}, {-0.5f, -0.5f, -0.5f, 1, 1}, {-0.5f, -0.5f, 0.5f, 0, 1},
	//RIGHT
	{0.5f, 0.5f, -0.5f, 0, 0}, {0.5f, 0.5f, 0.5f, 1, 0}, {0.5f, -0.5f, 0.5f, 1, 1},
	{0.5f, -0.5f, 0.5f, 1, 1}, {0.5f, -0.5f, -0.5f, 0, 1}, {0.5f, 0.5f, -0.5f, 0, 0}
	});

Cube::Cube(float size)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = Vec3(size, size, size);
	m_transform.UpdateMatrices();
	CreateConstantBuffer();
}

Cube::Cube(const Vec3& scale)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = scale;
	m_transform.UpdateMatrices();
	CreateConstantBuffer();
}

void Cube::CreateVertexBuffer()
{
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(s_mesh.MeshSize(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = s_mesh.GetPointerToVertices();
	HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, Cube::s_vertexBuffer.reset());
	ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
}

void Cube::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, s_mesh.POSITION_FORMAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, s_mesh.TEXTURE_FORMAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT result = engine::s_device->CreateInputLayout(layout, 2, s_vertexShaderBlob->GetBufferPointer(), s_vertexShaderBlob->GetBufferSize(), s_inputLayout.reset());
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

bool Cube::math_intersection(math::Intersection& nearest, const ray& r) const
{
	Vec3 v0, v1, v2;
	ray rr = r;
	rr.direction.mult(m_transform.transformInv, 0);
	rr.origin.mult(m_transform.transformInv, 1);
	int k = 0;
	for (int i = 0; i < 36; i += 3)
	{
		v0 = Vec3(s_mesh[i].coord[0], s_mesh[i].coord[1], s_mesh[i].coord[2]);
		v1 = Vec3(s_mesh[i + 1].coord[0], s_mesh[i + 1].coord[1], s_mesh[i + 1].coord[2]);
		v2 = Vec3(s_mesh[i + 2].coord[0], s_mesh[i + 2].coord[1], s_mesh[i + 2].coord[2]);
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

void Cube::TranslateRelative(const Vec3& dv)
{
	m_transform.position += dv.x() * m_transform.right() + dv.y() * m_transform.top() + dv.z() * m_transform.forward();
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::Rotate(const Quaternion& q)
{
	m_transform.rotation *= q;
	m_transform.rotation.normalize();
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::Rotate(const Angles& angles, const Vec3& _right, const Vec3& _top, const Vec3& _forward)
{
	m_transform.rotation *= Quaternion(angles.roll, _forward);
	m_transform.rotation *= Quaternion(angles.pitch, _right);
	m_transform.rotation *= Quaternion(angles.yaw, _top);
	m_transform.rotation.normalize();
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::Scale(const Vec3& scale)
{
	m_transform.scale *= scale;
	m_transform.UpdateMatrices();
	UpdateTransformMatrixBuffer();
}

void Cube::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(s_inputLayout.ptr());
	engine::s_deviceContext->VSSetConstantBuffers(1, 1, &m_transformMatrixBuffer.ptr());
	engine::s_deviceContext->VSSetShader(s_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(s_pixelShader.ptr(), nullptr, 0);

	engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

	UINT stride = s_mesh.VertexSize();
	UINT offset = 0;
	engine::s_deviceContext->IASetVertexBuffers(0, 1, &Cube::s_vertexBuffer.ptr(), &stride, &offset);
	engine::s_deviceContext->Draw(36, 0);
}



