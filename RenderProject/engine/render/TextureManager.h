#pragma once
#include "DxRes.hpp"
#include <map>
#include <string>

namespace engine
{
	class TextureManager
	{
		engine::DxResPtr<ID3D11Resource> m_texture;
		
		std::map<std::string, engine::DxResPtr<ID3D11SamplerState> > m_samplerState;
		std::map<std::string, engine::DxResPtr<ID3D11ShaderResourceView> > m_shaderResourceView;

		std::string m_globalSamplerStateKey;

		static TextureManager m_instance;
		TextureManager() {}
	public:
		static TextureManager& instance();
		void InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		void InitTexture(const std::wstring& fileName, const std::string& textureKey);

		void SetSamplerState(const std::string& samplerStateKey);
		void SetTexture(const std::string& textureKey);

		inline void SetGlobalSamplerStateKey(const std::string& _globalSamplerStateKey) { m_globalSamplerStateKey = _globalSamplerStateKey; }
		inline const std::string& GetGlobalSamplerStateKey() const { return m_globalSamplerStateKey; }
	};
}