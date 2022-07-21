#pragma once
#include "DxRes.hpp"
#include <map>
#include <string>

namespace engine
{
	class ShaderManager
	{
		std::map<std::string, std::pair<engine::DxResPtr<ID3D11VertexShader>, engine::DxResPtr<ID3D11PixelShader>> > m_shader;
		std::map<std::string, std::pair<engine::DxResPtr<ID3D10Blob>, engine::DxResPtr<ID3D10Blob>> > m_shaderBlob;

		static ShaderManager *s_instance;
		ShaderManager() {}
	public:
		static void init();
		static void deinit();
		static ShaderManager& instance();
		
		void InitShaders(const std::wstring& path, const std::string &shaderKey, UINT Flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);

		void GetShaders(const std::string& shaderKey, engine::DxResPtr<ID3D11VertexShader>& vertexShader, engine::DxResPtr<ID3D11PixelShader>& pixelShader);

		void GetShaderBlobs(const std::string& shaderKey, engine::DxResPtr<ID3D10Blob>& vertexShaderBlob, engine::DxResPtr<ID3D10Blob>& pixelShaderBlob);
	};
}