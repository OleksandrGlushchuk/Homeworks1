#pragma once
#include "../DxRes.hpp"
#include <unordered_map>
#include <string>
namespace engine
{
	class BlendStateManager
	{
		static BlendStateManager* s_instance;
		BlendStateManager() {}
		BlendStateManager(const BlendStateManager& other) = delete;
		BlendStateManager& operator=(const BlendStateManager& other) = delete;
		BlendStateManager(BlendStateManager&& other) noexcept = delete;
		BlendStateManager& operator=(BlendStateManager&& other) noexcept = delete;

		std::unordered_map<std::string, DxResPtr<ID3D11BlendState>> m_blendStates;

		void InitBlendStates();
	public:
		static void init();
		static void deinit();
		static BlendStateManager& instance();
		DxResPtr<ID3D11BlendState>& GetBlendState(const std::string& key);
	};
}