#include "BlendStateManager.h"

namespace engine
{
	BlendStateManager* BlendStateManager::s_instance;

	void BlendStateManager::InitBlendStates()
	{
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));

				D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc;
				rtBlendDesc.BlendEnable = TRUE;
				rtBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				rtBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
				rtBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
				rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtBlendDesc;

			HRESULT result = engine::s_device->CreateBlendState(&blendDesc, &m_blendStates["default"].ptr());
			ALWAYS_ASSERT(result >= 0 && "CreateBlendState");
		}
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.AlphaToCoverageEnable = TRUE;

				D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc;
				rtBlendDesc.BlendEnable = FALSE;
				rtBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				rtBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				rtBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
				rtBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
				rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtBlendDesc;

			HRESULT result = engine::s_device->CreateBlendState(&blendDesc, &m_blendStates["alphaToCoverage"].ptr());
			ALWAYS_ASSERT(result >= 0 && "CreateBlendState");
		}
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));

			D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc;
			rtBlendDesc.BlendEnable = TRUE;
			rtBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			rtBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
			rtBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			rtBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
			rtBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

			blendDesc.RenderTarget[0] = rtBlendDesc;

			HRESULT result = engine::s_device->CreateBlendState(&blendDesc, &m_blendStates["additive"].ptr());
			ALWAYS_ASSERT(result >= 0 && "CreateBlendState");
		}
	}
	void BlendStateManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new BlendStateManager;
		s_instance->InitBlendStates();
	}
	void BlendStateManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}
	BlendStateManager& BlendStateManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
	DxResPtr<ID3D11BlendState>& BlendStateManager::GetBlendState(const std::string& key)
	{
		std::unordered_map< std::string, engine::DxResPtr<ID3D11BlendState> >::iterator result;
		ALWAYS_ASSERT((result = m_blendStates.find(key)) != m_blendStates.end() && "Bad blendStateKey");
		return result->second;
	}
}
