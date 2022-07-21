#include "ShaderManager.h"

namespace engine
{
	ShaderManager *ShaderManager::s_instance;

	void ShaderManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ShaderManager;
	}

	void ShaderManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	ShaderManager& ShaderManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void ShaderManager::InitShaders(const std::wstring& path, const std::string& shaderKey, UINT Flags1)
	{
		engine::DxResPtr<ID3DBlob> error;

		auto& vertexShader = m_shader[shaderKey].first;
		auto& vertexShaderBlob = m_shaderBlob[shaderKey].first;

		auto& pixelShader = m_shader[shaderKey].second;
		auto& pixelShaderBlob = m_shaderBlob[shaderKey].second;

		HRESULT result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", Flags1, 0, vertexShaderBlob.reset(), error.reset());
		ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

		result = engine::s_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, vertexShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");


		result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", Flags1, 0, pixelShaderBlob.reset(), error.reset());
		ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

		result = engine::s_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, pixelShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
	}

	void ShaderManager::GetShaders(const std::string& shaderKey, engine::DxResPtr<ID3D11VertexShader>& vertexShader, engine::DxResPtr<ID3D11PixelShader>& pixelShader)
	{
		std::map<std::string, std::pair<engine::DxResPtr<ID3D11VertexShader>, engine::DxResPtr<ID3D11PixelShader>> >::iterator result;
		ALWAYS_ASSERT((result = m_shader.find(shaderKey)) != m_shader.end() && "Bad shaderKey");
		vertexShader = result->second.first;
		pixelShader = result->second.second;
	}

	void ShaderManager::GetShaderBlobs(const std::string& shaderKey, engine::DxResPtr<ID3D10Blob>& vertexShaderBlob, engine::DxResPtr<ID3D10Blob>& pixelShaderBlob)
	{
		std::map<std::string, std::pair<engine::DxResPtr<ID3D10Blob>, engine::DxResPtr<ID3D10Blob>> >::iterator result;
		ALWAYS_ASSERT((result = m_shaderBlob.find(shaderKey)) != m_shaderBlob.end() && "Bad shaderKey");
		vertexShaderBlob = result->second.first;
		pixelShaderBlob = result->second.second;
	}

}