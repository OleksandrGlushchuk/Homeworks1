#pragma once
#include "DxRes.hpp"
#include <vector>
template<typename Data>
class ConstantBuffer
{
	engine::DxResPtr<ID3D11Buffer> m_constantBuffer;
public:
	ConstantBuffer() {}

	inline void Init(D3D11_USAGE usage, UINT cpuAccessFlags)
	{
		auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(Data), D3D11_BIND_CONSTANT_BUFFER, usage, cpuAccessFlags);
		HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, nullptr, m_constantBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	inline void Init(D3D11_USAGE usage, UINT cpuAccessFlags, const Data* pData)
	{
		auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(Data), D3D11_BIND_CONSTANT_BUFFER, usage, cpuAccessFlags);
		D3D11_SUBRESOURCE_DATA constantBufferData = { 0 };
		constantBufferData.pSysMem = pData;
		HRESULT result = engine::s_device->CreateBuffer(&vertexBufferDesc, &constantBufferData, m_constantBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	inline D3D11_MAPPED_SUBRESOURCE Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT result = engine::s_deviceContext->Map(m_constantBuffer.ptr(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ALWAYS_ASSERT(result >= 0 && "Map");
		return mappedSubresource;
	}
	inline void Unmap()
	{
		engine::s_deviceContext->Unmap(m_constantBuffer.ptr(), 0);
	}

	inline void Bind(uint32_t startSlot = 0)
	{
		engine::s_deviceContext->VSSetConstantBuffers(startSlot, 1, &m_constantBuffer.ptr());
	}
};