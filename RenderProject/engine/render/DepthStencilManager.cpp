#include "DepthStencilManager.h"

namespace engine
{
	DepthStencilManager* DepthStencilManager::s_instance;

	void DepthStencilManager::InitDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;

		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		CreateDepthStencilState(depthStencilDesc, "default");
	}

	void DepthStencilManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new DepthStencilManager;
		s_instance->InitDepthStencilStates();
	}

	void DepthStencilManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	void DepthStencilManager::CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& desc, const std::string& depthStencilkey)
	{
		HRESULT result = engine::s_device->CreateDepthStencilState(&desc, &m_depthStencilState[depthStencilkey].ptr());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilState");
	}

	DxResPtr<ID3D11DepthStencilState>& DepthStencilManager::GetDepthStencilState(const std::string& depthStencilkey)
	{
		std::unordered_map< std::string, engine::DxResPtr<ID3D11DepthStencilState> >::iterator result;
		ALWAYS_ASSERT((result = m_depthStencilState.find(depthStencilkey)) != m_depthStencilState.end() && "Bad depthStencilkey");
		return result->second;
	}

	DepthStencilManager& DepthStencilManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
}
