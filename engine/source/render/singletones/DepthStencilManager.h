#pragma once
#include "../DxRes.hpp"
#include <unordered_map>
#include <string> 
namespace engine
{
	class DepthStencilManager
	{
		static DepthStencilManager* s_instance;

		std::unordered_map<std::string, DxResPtr<ID3D11DepthStencilState> > m_depthStencilState;
		DepthStencilManager() {}
		DepthStencilManager(const DepthStencilManager& other) = delete;
		DepthStencilManager& operator=(const DepthStencilManager& other) = delete;
		DepthStencilManager(DepthStencilManager&& other) noexcept = delete;
		DepthStencilManager& operator=(DepthStencilManager&& other) noexcept = delete;

		void InitDepthStencilStates();
	public:
		static void init();
		static void deinit();
		void CreateDepthStencilState(D3D11_DEPTH_STENCIL_DESC& desc, const std::string& depthStencilkey);
		DxResPtr<ID3D11DepthStencilState>& GetDepthStencilState(const std::string& depthStencilkey);
		static DepthStencilManager& instance();
	};
}