#pragma once
#include "DxRes.hpp"
#include <vector>
template<typename VertexType>
class VertexBuffer
{
	engine::DxResPtr<ID3D11Buffer> m_vertexBuffer;
	uint32_t size;
public:
	VertexBuffer(){}

	inline uint32_t Size() const { return size; }

	inline void Init(uint32_t num_elements, D3D11_USAGE usage, UINT cpuAccessFlags)
	{
		size = num_elements;
		if (size == 0)
		{
			m_vertexBuffer.reset();
			return;
		}
		auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(VertexType) * num_elements, D3D11_BIND_VERTEX_BUFFER, usage, cpuAccessFlags);
		HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, nullptr, m_vertexBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	inline void Init(const std::vector<VertexType> &vertices)
	{
		size = vertices.size();
		if (size == 0)
		{
			m_vertexBuffer.reset();
			return;
		}
		auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(VertexType) * vertices.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = vertices.data();
		HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	inline D3D11_MAPPED_SUBRESOURCE Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT result = engine::s_deviceContext->Map(m_vertexBuffer.ptr(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ALWAYS_ASSERT(result >= 0 && "Map");
		return mappedSubresource;
	}
	inline void Unmap()
	{
		engine::s_deviceContext->Unmap(m_vertexBuffer.ptr(), 0);
	}

	inline void Bind(uint32_t startSlot = 0)
	{
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		engine::s_deviceContext->IASetVertexBuffers(startSlot, 1, &m_vertexBuffer.ptr(), &stride, &offset);
	}
};

