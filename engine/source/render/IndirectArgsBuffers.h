#pragma once
#include "DxRes.hpp"

namespace engine
{
	struct InstancedIndirectArgsBuffer
	{
		DxResPtr<ID3D11Buffer> m_buffer;
		D3D11_BUFFER_DESC m_bufferDesc;

		InstancedIndirectArgsBuffer() {}
		void Init()
		{
			m_bufferDesc.ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
			m_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			m_bufferDesc.CPUAccessFlags = 0;
			m_bufferDesc.StructureByteStride = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
			m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

			D3D11_SUBRESOURCE_DATA subresource = { 0 };
			D3D11_DRAW_INSTANCED_INDIRECT_ARGS data = { 0 };
			subresource.pSysMem = &data;

			HRESULT result = s_device->CreateBuffer(&m_bufferDesc, &subresource, m_buffer.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
		}
	};

	struct IndexedInstancedIndirectArgsBuffer
	{
		DxResPtr<ID3D11Buffer> m_buffer;
		D3D11_BUFFER_DESC m_bufferDesc;

		IndexedInstancedIndirectArgsBuffer() {}
		void Init()
		{
			m_bufferDesc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
			m_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			m_bufferDesc.CPUAccessFlags = 0;
			m_bufferDesc.StructureByteStride = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
			m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

			D3D11_SUBRESOURCE_DATA subresource = { 0 };
			D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS data = { 0 };
			subresource.pSysMem = &data;

			HRESULT result = s_device->CreateBuffer(&m_bufferDesc, &subresource, m_buffer.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
		}
	};
}