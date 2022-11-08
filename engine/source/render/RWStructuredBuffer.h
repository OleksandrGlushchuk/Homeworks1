#pragma once
#include "DxRes.hpp"

namespace engine
{
	template<typename TYPE>
	struct RWStructuredBuffer
	{
		DxResPtr<ID3D11Buffer> m_buffer;
		D3D11_BUFFER_DESC m_bufferDesc;
		DxResPtr<ID3D11UnorderedAccessView> m_uav;
		D3D11_UNORDERED_ACCESS_VIEW_DESC m_uavDesc;

		RWStructuredBuffer(){}
		void Init(uint32_t numElements, const TYPE *p_initData)
		{
			m_bufferDesc.ByteWidth = sizeof(TYPE) * numElements;
			m_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			m_bufferDesc.CPUAccessFlags = 0;
			m_bufferDesc.StructureByteStride = sizeof(TYPE);
			m_bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			
			D3D11_SUBRESOURCE_DATA subresource = {0};
			subresource.pSysMem = p_initData;

			HRESULT result = s_device->CreateBuffer(&m_bufferDesc, p_initData ? &subresource : nullptr, m_buffer.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");

			
			m_uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			m_uavDesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			m_uavDesc.Buffer.FirstElement = 0;
			m_uavDesc.Buffer.NumElements = numElements;
			m_uavDesc.Buffer.Flags = 0;
			result = s_device->CreateUnorderedAccessView(m_buffer.ptr(), &m_uavDesc, m_uav.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateUnorderedAccessView");
		}


	};
}