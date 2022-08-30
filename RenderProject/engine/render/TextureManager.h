#pragma once
#include "DxRes.hpp"
#include <unordered_map>
#include <string>

namespace engine
{
	class TextureManager
	{
		engine::DxResPtr<ID3D11Resource> m_colorMap;
		std::unordered_map<std::wstring, engine::DxResPtr<ID3D11ShaderResourceView> > m_shaderResourceView;
		static TextureManager *s_instance;
		TextureManager() {}
		TextureManager(const TextureManager& other) = delete;
		TextureManager& operator=(const TextureManager& other) = delete;
		TextureManager(TextureManager&& other) noexcept = delete;
		TextureManager& operator=(TextureManager&& other) noexcept = delete;
	public:
		static void init();
		static void deinit();
		static TextureManager& instance();
		const engine::DxResPtr<ID3D11ShaderResourceView>& LoadTexture(const std::wstring& fileName, bool need_to_force_sRGB = false);

		const engine::DxResPtr<ID3D11ShaderResourceView>& GetTexture(const std::wstring& fileName);
	};
}