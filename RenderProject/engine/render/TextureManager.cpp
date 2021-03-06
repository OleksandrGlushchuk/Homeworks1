#include "TextureManager.h"
#include "../DDSTextureLoader/DDSTextureLoader11.h"
#include "globals.hpp"

namespace engine
{
	TextureManager *TextureManager::s_instance;

	void TextureManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new TextureManager;
	}

	void TextureManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	TextureManager& TextureManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void TextureManager::InitTexture(const std::wstring& fileName, const std::string& textureKey)
	{
		HRESULT result = DirectX::CreateDDSTextureFromFile(engine::s_device, engine::s_deviceContext, fileName.c_str(), m_texture.reset(), m_shaderResourceView[textureKey].reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDDSTextureFromFile");
	}

	const engine::DxResPtr<ID3D11ShaderResourceView>& TextureManager::GetTexture(const std::string& textureKey)
	{
		std::map< std::string, engine::DxResPtr<ID3D11ShaderResourceView> >::iterator result;
		ALWAYS_ASSERT((result = m_shaderResourceView.find(textureKey)) != m_shaderResourceView.end() && "Bad textureKey");
		return result->second;
	}
}