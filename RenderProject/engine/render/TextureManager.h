#pragma once
#include "DxRes.hpp"
#include <map>
#include <string>

namespace engine
{
	class TextureManager
	{
		engine::DxResPtr<ID3D11Resource> m_texture;
		std::map<std::string, engine::DxResPtr<ID3D11ShaderResourceView> > m_shaderResourceView;

		static TextureManager *s_instance;
		TextureManager() {}
	public:
		static void init();
		static void deinit();
		static TextureManager& instance();
		void InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		void InitTexture(const std::wstring& fileName, const std::string& textureKey);

		void SetGlobalSamplerState(const std::string& _globalSamplerStateKey);

		const engine::DxResPtr<ID3D11ShaderResourceView>& GetTexture(const std::string& textureKey);
	};
}