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

	void ShaderManager::InitComputeShader(const std::wstring& path, UINT Flags1)
	{
		engine::DxResPtr<ID3DBlob> error;

		auto& computeShader = m_computeShader[path];
		auto& computeShaderBlob = m_computeShaderBlob[path];

		HRESULT result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main", "cs_5_0", Flags1, 0, computeShaderBlob.reset(), error.reset());
		ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

		result = engine::s_device->CreateComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize(), nullptr, computeShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateComputeShader");
	}

	void ShaderManager::GetComputeShader(const std::wstring& path, engine::DxResPtr<ID3D11ComputeShader>& computeShader)
	{
		std::unordered_map<std::wstring, DxResPtr<ID3D11ComputeShader> >::iterator result;
		ALWAYS_ASSERT((result = m_computeShader.find(path)) != m_computeShader.end() && "Bad path");
		computeShader = result->second;
	}

	void ShaderManager::GetComputeShaderBlob(const std::wstring& path, engine::DxResPtr<ID3D10Blob>& computeShaderBlob)
	{
		std::unordered_map<std::wstring, DxResPtr<ID3D10Blob> >::iterator result;
		ALWAYS_ASSERT((result = m_computeShaderBlob.find(path)) != m_computeShaderBlob .end() && "Bad path");
		computeShaderBlob = result->second;
	}

	void ShaderManager::InitShaders(const std::wstring& path, UINT Flags1)
	{
		engine::DxResPtr<ID3DBlob> error;

		auto& vertexShader = m_shader[path].first;
		auto& vertexShaderBlob = m_shaderBlob[path].first;

		auto& pixelShader = m_shader[path].second;
		auto& pixelShaderBlob = m_shaderBlob[path].second;

		HRESULT result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", Flags1, 0, vertexShaderBlob.reset(), error.reset());
		ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

		result = engine::s_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, vertexShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");


		result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", Flags1, 0, pixelShaderBlob.reset(), error.reset());
		ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");

		result = engine::s_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, pixelShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
	}

	void ShaderManager::GetShaders(const std::wstring& shaderKey, engine::DxResPtr<ID3D11VertexShader>& vertexShader, engine::DxResPtr<ID3D11PixelShader>& pixelShader)
	{
		std::unordered_map<std::wstring, std::pair<engine::DxResPtr<ID3D11VertexShader>, engine::DxResPtr<ID3D11PixelShader>> >::iterator result;
		ALWAYS_ASSERT((result = m_shader.find(shaderKey)) != m_shader.end() && "Bad shaderKey");
		vertexShader = result->second.first;
		pixelShader = result->second.second;
	}

	void ShaderManager::GetShaderBlobs(const std::wstring& shaderKey, engine::DxResPtr<ID3D10Blob>& vertexShaderBlob, engine::DxResPtr<ID3D10Blob>& pixelShaderBlob)
	{
		std::unordered_map<std::wstring, std::pair<engine::DxResPtr<ID3D10Blob>, engine::DxResPtr<ID3D10Blob>> >::iterator result;
		ALWAYS_ASSERT((result = m_shaderBlob.find(shaderKey)) != m_shaderBlob.end() && "Bad shaderKey");
		vertexShaderBlob = result->second.first;
		pixelShaderBlob = result->second.second;
	}

}