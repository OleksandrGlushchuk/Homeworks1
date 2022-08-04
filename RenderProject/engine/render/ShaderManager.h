#pragma once
#include "DxRes.hpp"
#include <unordered_map>
#include <string>

namespace engine
{
	class ShaderManager
	{
		std::unordered_map<std::wstring, std::pair<engine::DxResPtr<ID3D11VertexShader>, engine::DxResPtr<ID3D11PixelShader>> > m_shader;
		std::unordered_map<std::wstring, std::pair<engine::DxResPtr<ID3D10Blob>, engine::DxResPtr<ID3D10Blob>> > m_shaderBlob;
		std::unordered_map<std::wstring, engine::DxResPtr<ID3D11ComputeShader> > m_computeShader;
		std::unordered_map<std::wstring, engine::DxResPtr<ID3D10Blob> > m_computeShaderBlob;

		static ShaderManager *s_instance;
		ShaderManager() {}
		ShaderManager(const ShaderManager& other) = delete;
		ShaderManager& operator=(const ShaderManager& other) = delete;
		ShaderManager(ShaderManager&& other) noexcept = delete;
		ShaderManager& operator=(ShaderManager&& other) noexcept = delete;
	public:
		static void init();
		static void deinit();
		static ShaderManager& instance();
		
		void InitComputeShader(const std::wstring& path, UINT Flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
		void GetComputeShader(const std::wstring& path, engine::DxResPtr<ID3D11ComputeShader> &computeShader);
		void GetComputeShaderBlob(const std::wstring& path, engine::DxResPtr<ID3D10Blob>& computeShaderBlob);

		void InitShaders(const std::wstring& path, UINT Flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);

		void GetShaders(const std::wstring& shaderKey, engine::DxResPtr<ID3D11VertexShader>& vertexShader, engine::DxResPtr<ID3D11PixelShader>& pixelShader);

		void GetShaderBlobs(const std::wstring& shaderKey, engine::DxResPtr<ID3D10Blob>& vertexShaderBlob, engine::DxResPtr<ID3D10Blob>& pixelShaderBlob);
	};
}