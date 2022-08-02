#pragma once
#include "DxRes.hpp"
#include <vector>
#include "TriangleIndices.h"

class IndexBuffer
{
	engine::DxResPtr<ID3D11Buffer> m_indexBuffer;
	uint32_t size;
public:

	template<typename IndexType>
	void Init(const std::vector<IndexType>& indices)
	{
		CD3D11_BUFFER_DESC indexBufferDesc;
		if constexpr (std::is_same<TriangleIndices, IndexType>::value)
		{
			size = 3 * indices.size();
			indexBufferDesc = CD3D11_BUFFER_DESC(3 * sizeof(uint32_t) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
		}
		else if constexpr (std::is_integral<IndexType>::value)
		{
			size = indices.size();
			indexBufferDesc = CD3D11_BUFFER_DESC(sizeof(IndexType) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
		}
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indices.data();
		HRESULT result = engine::s_device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}
	void Bind()
	{
		engine::s_deviceContext->IASetIndexBuffer(m_indexBuffer.ptr(), DXGI_FORMAT_R32_UINT, 0);
	}
	inline uint32_t Size() const { return size; }

};
