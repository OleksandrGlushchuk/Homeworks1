#pragma once
#include "../DxRes.hpp"
#include <unordered_map>
#include <string>
#include <tuple>
#include <array>

namespace engine
{
	struct ShaderEnabling
	{
		bool hasPS : 1;
		bool hasGS : 1;
		ShaderEnabling(){}
		ShaderEnabling(bool _hasPS, bool _hasGS) : hasPS(_hasPS), hasGS(_hasGS) {}
	};
	class ShaderManager
	{
		std::unordered_map<std::wstring, std::array<DxResPtr<ID3D10Blob>,3> > m_shaderBlob;
		std::unordered_map<std::wstring, std::tuple<DxResPtr<ID3D11VertexShader>, 
			DxResPtr<ID3D11PixelShader>, DxResPtr<ID3D11GeometryShader>> > m_shader;

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

		void InitShaders(const std::wstring& path, const ShaderEnabling& shaderEnabling, UINT Flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR);

		void GetShaders(const std::wstring& shaderKey, DxResPtr<ID3D11VertexShader> &vertexShader, 
			DxResPtr<ID3D11PixelShader> &pixelShader, DxResPtr<ID3D11GeometryShader> &geomytryShader);

		void GetShaderBlobs(const std::wstring& shaderKey, DxResPtr<ID3D10Blob> &vertexShaderBlob, 
			DxResPtr<ID3D10Blob> &pixelShaderBlob, DxResPtr<ID3D10Blob> &geometryShaderBlob);
	};
}