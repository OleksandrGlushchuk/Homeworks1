#include "TextureManager.h"
#include "../../DDSTextureLoader/DDSTextureLoader11.h"
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

	const engine::DxResPtr<ID3D11ShaderResourceView>& TextureManager::LoadTexture(const std::wstring& fileName, 
		DxResPtr<ID3D11Texture2D>& out_resource, bool need_to_force_sRGB)
	{
		std::unordered_map<std::wstring, std::pair<engine::DxResPtr<ID3D11ShaderResourceView>, DxResPtr<ID3D11Resource> >>::iterator find_it;
		if ((find_it = m_SRVandResource.find(fileName)) != m_SRVandResource.end())
		{
			HRESULT result = find_it->second.second->QueryInterface(IID_ID3D11Texture2D, (void**)out_resource.reset());
			ALWAYS_ASSERT(result >= 0 && "QueryInterface");
			return find_it->second.first;
		}
		auto& SRVandResource = m_SRVandResource[fileName];
		if (!need_to_force_sRGB)
		{
			HRESULT result = DirectX::CreateDDSTextureFromFile(engine::s_device, engine::s_deviceContext, fileName.c_str(), SRVandResource.second.reset(), SRVandResource.first.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateDDSTextureFromFile");
		}
		else
		{
			HRESULT result = DirectX::CreateDDSTextureFromFileEx(engine::s_device, engine::s_deviceContext,	fileName.c_str(), 0,
				D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				DirectX::DDS_LOADER_FORCE_SRGB,
				SRVandResource.second.reset(), SRVandResource.first.reset(), nullptr);
			
			ALWAYS_ASSERT(result >= 0 && "CreateDDSTextureFromFile");
		}
		HRESULT result = SRVandResource.second.ptr()->QueryInterface(IID_ID3D11Texture2D, (void**)out_resource.reset());
		ALWAYS_ASSERT(result >= 0 && "QueryInterface");
		return SRVandResource.first;

	}
}