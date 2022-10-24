#pragma once
#include "../DxRes.hpp"
#include <unordered_map>
#include <string> 

namespace engine
{
	class RasteriserStateManager
	{
		static RasteriserStateManager* s_instance;

		RasteriserStateManager() {}
		RasteriserStateManager(const RasteriserStateManager& other) = delete;
		RasteriserStateManager& operator=(const RasteriserStateManager& other) = delete;
		RasteriserStateManager(RasteriserStateManager&& other) noexcept = delete;
		RasteriserStateManager& operator=(RasteriserStateManager&& other) noexcept = delete;

		std::unordered_map<std::string, DxResPtr<ID3D11RasterizerState>> m_rasteriserState;
		void InitRasteriserStates();
	public:
		static void init();
		static void deinit();
		static RasteriserStateManager& instance();
		DxResPtr<ID3D11RasterizerState>& GetRasteriserState(const std::string& key);
	};
}