#pragma once
#include "../DxRes.hpp"
#include <unordered_map>
#include <string>
namespace engine
{
	class SamplerManager
	{
		static SamplerManager* s_instance;
		std::unordered_map<std::string, DxResPtr<ID3D11SamplerState> > m_samplerState;
		DxResPtr<ID3D11SamplerState> m_globalSamplerState;
		void initSamplerStates();

		SamplerManager() {}
		SamplerManager(const SamplerManager& other) = delete;
		SamplerManager& operator=(const SamplerManager& other) = delete;
		SamplerManager(SamplerManager&& other) noexcept = delete;
		SamplerManager& operator=(SamplerManager&& other) noexcept = delete;
	public:
		static void init();
		static void deinit();
		static SamplerManager& instance();
		void CreateSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		DxResPtr<ID3D11SamplerState>& GetSamplerState(const std::string& samplerStateKey);
		DxResPtr<ID3D11SamplerState>& GetGlobalSamplerState();
		void SetGlobalSamplerState(const std::string& samplerStateKey);
	};
}