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

	void ShaderManager::InitShaders(const std::wstring& path, const ShaderEnabling& shaderEnabling, UINT Flags1)
	{
		std::unordered_map<std::wstring, std::tuple<DxResPtr<ID3D11VertexShader>,
			DxResPtr<ID3D11PixelShader>, DxResPtr<ID3D11GeometryShader>,
			DxResPtr<ID3D11HullShader>, DxResPtr<ID3D11DomainShader>> >::iterator res;

		if ((res = m_shader.find(path)) != m_shader.end())
			return;
		HRESULT result;
		engine::DxResPtr<ID3DBlob> error;

		auto& vertexShader = std::get<0>(m_shader[path]);
		auto& vertexShaderBlob = m_shaderBlob[path][0];

		result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", Flags1, 0, vertexShaderBlob.reset(), error.reset());
		if (result < 0)
		{
			OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
			__debugbreak();
			abort();
		}

		result = engine::s_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, vertexShader.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateVertexShader");

		if (shaderEnabling.hasPS)
		{
			auto& pixelShader = std::get<1>(m_shader[path]);
			auto& pixelShaderBlob = m_shaderBlob[path][1];

			result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", Flags1, 0, pixelShaderBlob.reset(), error.reset());
			if (result < 0)
			{
				OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
				__debugbreak();
				abort();
			}

			result = engine::s_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, pixelShader.reset());
			ALWAYS_ASSERT(result >= 0 && "CreatePixelShader");
		}

		if (shaderEnabling.hasGS)
		{
			auto& geometryShader = std::get<2>(m_shader[path]);
			auto& geometryShaderBlob = m_shaderBlob[path][2];

			result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "gs_main", "gs_5_0", Flags1, 0, geometryShaderBlob.reset(), error.reset());
			if (result < 0)
			{
				OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
				__debugbreak();
				abort();
			}

			result = engine::s_device->CreateGeometryShader(geometryShaderBlob->GetBufferPointer(), geometryShaderBlob->GetBufferSize(), nullptr, geometryShader.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateGeometryShader");
		}

		if (shaderEnabling.hasHSDS)
		{
			{
				auto& hullShader = std::get<3>(m_shader[path]);
				auto& hullShaderBlob = m_shaderBlob[path][3];
				result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "hs_main", "hs_5_0", Flags1, 0, hullShaderBlob.reset(), error.reset());
				if (result < 0)
				{
					OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
					__debugbreak();
					abort();
				}
				result = engine::s_device->CreateHullShader(hullShaderBlob->GetBufferPointer(), hullShaderBlob->GetBufferSize(), nullptr, hullShader.reset());
				ALWAYS_ASSERT(result >= 0 && "CreateGeometryShader");
			}

			{
				auto& domainShader = std::get<4>(m_shader[path]);
				auto& domainShaderBlob = m_shaderBlob[path][4];
				result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ds_main", "ds_5_0", Flags1, 0, domainShaderBlob.reset(), error.reset());
				if (result < 0)
				{
					OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
					__debugbreak();
					abort();
				}
				result = engine::s_device->CreateDomainShader(domainShaderBlob->GetBufferPointer(), domainShaderBlob->GetBufferSize(), nullptr, domainShader.reset());
				ALWAYS_ASSERT(result >= 0 && "CreateGeometryShader");
			}
		}
	}

	void ShaderManager::InitCompute(const std::wstring& path, UINT Flags1)
	{
		std::unordered_map<std::wstring, DxResPtr<ID3D11ComputeShader> >::iterator it;
		if ((it = m_computeShader.find(path)) != m_computeShader.end())
			return;

		engine::DxResPtr<ID3DBlob> error;
		DxResPtr<ID3D10Blob> blob;

		HRESULT result = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main", "cs_5_0", Flags1, 0, blob.reset(), error.reset());
		//ALWAYS_ASSERT(result >= 0 && "D3DCompileFromFile");
		if (result < 0)
		{
			OutputDebugStringA(LPCSTR(error->GetBufferPointer()));
			__debugbreak();
			abort();
		}

		result = s_device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_computeShader[path].reset());
	}

	void ShaderManager::GetCompute(const std::wstring& path, DxResPtr<ID3D11ComputeShader>& computeShader)
	{
		std::unordered_map<std::wstring, DxResPtr<ID3D11ComputeShader> >::iterator result;
		ALWAYS_ASSERT((result = m_computeShader.find(path)) != m_computeShader.end() && "Bad path");

		computeShader = m_computeShader[path];
	}

	void ShaderManager::GetShaders(const std::wstring& shaderKey, DxResPtr<ID3D11VertexShader>& vertexShader,
		DxResPtr<ID3D11PixelShader>& pixelShader, DxResPtr<ID3D11GeometryShader>& geomytryShader,
		DxResPtr<ID3D11HullShader>& hullShader, DxResPtr<ID3D11DomainShader>& domainShader)
	{
		std::unordered_map<std::wstring, std::tuple<DxResPtr<ID3D11VertexShader>,
			DxResPtr<ID3D11PixelShader>, DxResPtr<ID3D11GeometryShader>,
			DxResPtr<ID3D11HullShader>, DxResPtr<ID3D11DomainShader>> >::iterator result;
		ALWAYS_ASSERT((result = m_shader.find(shaderKey)) != m_shader.end() && "Bad shaderKey");

		vertexShader = std::get<0>(result->second);

		if(std::get<1>(result->second).ptr())
			pixelShader = std::get<1>(result->second);
		if(std::get<2>(result->second).ptr())
			geomytryShader = std::get<2>(result->second);
		if (std::get<3>(result->second).ptr())
			hullShader = std::get<3>(result->second);
		if (std::get<4>(result->second).ptr())
			domainShader = std::get<4>(result->second);
	}

	void ShaderManager::GetShaderBlobs(const std::wstring& shaderKey, DxResPtr<ID3D10Blob>& vertexShaderBlob,
		DxResPtr<ID3D10Blob>& pixelShaderBlob, DxResPtr<ID3D10Blob>& geometryShaderBlob,
		DxResPtr<ID3D10Blob>& hullShaderBlob, DxResPtr<ID3D10Blob>& domainShaderBlob)
	{
		std::unordered_map<std::wstring, std::array<DxResPtr<ID3D10Blob>, 5> >::iterator result;
		ALWAYS_ASSERT((result = m_shaderBlob.find(shaderKey)) != m_shaderBlob.end() && "Bad shaderKey");

		vertexShaderBlob = std::get<0>(result->second);

		if (std::get<1>(result->second).ptr())
			pixelShaderBlob = std::get<1>(result->second);
		if (std::get<2>(result->second).ptr())
			geometryShaderBlob = std::get<2>(result->second);
		if (std::get<3>(result->second).ptr())
			hullShaderBlob = std::get<3>(result->second);
		if (std::get<4>(result->second).ptr())
			domainShaderBlob = std::get<4>(result->second);
	}

}