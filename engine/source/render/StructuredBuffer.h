#pragma once
//#include "DxRes.hpp"
//#include <vector>
//
//namespace engine
//{
//	template<typename Data>
//	class StructuredBuffer
//	{
//		engine::DxResPtr<ID3D11Buffer> m_structuredBuffer;
//		DxResPtr<ID3D11UnorderedAccessView> m_uav;
//	public:
//		StructuredBuffer() {}
//
//		inline void Init(D3D11_USAGE usage, UINT cpuAccessFlags)
//		{
//			auto structuredBufferDesc = CD3D11_BUFFER_DESC(sizeof(Data), D3D11_BIND_UNORDERED_ACCESS, usage, cpuAccessFlags, D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);
//			HRESULT result = engine::s_device->CreateBuffer(&structuredBufferDesc, nullptr, m_structuredBuffer.reset());
//			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
//
//			result = s_device->CreateUnorderedAccessView(m_structuredBuffer.ptr(), nullptr , m_uav.reset());
//			ALWAYS_ASSERT(result >= 0 && "CreateUnorderedAccessView");
//		}
//
//		inline void BindVS(uint32_t startSlot = 0)
//		{
//			engine::s_deviceContext-> ///VSSetShaderResources(0, 1, &m_uav.ptr());
//		}
//
//		//inline void BindPS(uint32_t startSlot = 0)
//		//{
//		//	engine::s_deviceContext->PSSetConstantBuffers(startSlot, 1, &m_constantBuffer.ptr());
//		//}
//
//		//inline void BindGS(uint32_t startSlot = 0)
//		//{
//		//	engine::s_deviceContext->GSSetConstantBuffers(startSlot, 1, &m_constantBuffer.ptr());
//		//}
//	};
//}