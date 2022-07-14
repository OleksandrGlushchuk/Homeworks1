#include "TextureManager.h"
#include "../DDSTextureLoader/DDSTextureLoader11.h"

namespace engine
{
	TextureManager TextureManager::m_instance;
	TextureManager& TextureManager::instance()
	{
		return m_instance;
	}
	void TextureManager::InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey)
	{
		HRESULT result = engine::s_device->CreateSamplerState(&samplerDesc, m_samplerState[samplerStateKey].reset());
		ALWAYS_ASSERT(result >= 0 && "CreateSamplerState");
	}

	void TextureManager::InitTexture(const std::wstring& fileName, const std::string& textureKey)
	{
		HRESULT result = DirectX::CreateDDSTextureFromFile(engine::s_device, engine::s_deviceContext, fileName.c_str(), m_texture.reset(), m_shaderResourceView[textureKey].reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDDSTextureFromFile");
	}
	void TextureManager::SetSamplerState(const std::string& samplerStateKey)
	{
		ALWAYS_ASSERT(m_samplerState.find(samplerStateKey) != m_samplerState.end() && "Bad samplerStateKey");
		engine::s_deviceContext->PSSetSamplers(0, 1, &m_samplerState[samplerStateKey].ptr());
		
	}
	void TextureManager::SetTexture(const std::string& textureKey)
	{
		ALWAYS_ASSERT(m_shaderResourceView.find(textureKey) != m_shaderResourceView.end() && "Bad textureKey");
		engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView[textureKey].ptr());
	}
}