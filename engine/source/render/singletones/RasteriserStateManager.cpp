#include "RasteriserStateManager.h"

namespace engine
{
	RasteriserStateManager* RasteriserStateManager::s_instance;

	void RasteriserStateManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new RasteriserStateManager;
		s_instance->InitRasteriserStates();
	}
	void RasteriserStateManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}
	RasteriserStateManager& RasteriserStateManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void RasteriserStateManager::InitRasteriserStates()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AntialiasedLineEnable = TRUE;
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.MultisampleEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.SlopeScaledDepthBias = 0.f;

		HRESULT result = engine::s_device->CreateRasterizerState(&desc, &m_rasteriserState["front_and_back"].ptr());
		ALWAYS_ASSERT(result >= 0 && "CreateRasterizerState");

		desc.CullMode = D3D11_CULL_FRONT;
		result = engine::s_device->CreateRasterizerState(&desc, &m_rasteriserState["back"].ptr());
		ALWAYS_ASSERT(result >= 0 && "CreateRasterizerState");

		desc.DepthClipEnable = FALSE;
		result = engine::s_device->CreateRasterizerState(&desc, &m_rasteriserState["point_light"].ptr());
		ALWAYS_ASSERT(result >= 0 && "CreateRasterizerState");
	}
	DxResPtr<ID3D11RasterizerState>& RasteriserStateManager::GetRasteriserState(const std::string& key)
	{
		std::unordered_map< std::string, engine::DxResPtr<ID3D11RasterizerState> >::iterator result;
		ALWAYS_ASSERT((result = m_rasteriserState.find(key)) != m_rasteriserState.end() && "Bad rasteriserStateKey");
		return result->second;
	}
}