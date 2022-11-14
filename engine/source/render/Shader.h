#pragma once
#include "singletones/ShaderManager.h"
#include <string>
namespace engine
{
	class Shader
	{
		DxResPtr<ID3D11VertexShader> m_vertexShader;
		DxResPtr<ID3D11PixelShader> m_pixelShader;
		DxResPtr<ID3D11GeometryShader> m_geometryShader;
		DxResPtr<ID3D11HullShader> m_hullShader;
		DxResPtr<ID3D11DomainShader> m_domainShader;
		DxResPtr<ID3D11ComputeShader> m_computeShader;
		DxResPtr<ID3DBlob> m_vertexShaderBlob;
		DxResPtr<ID3DBlob> m_pixelShaderBlob;
		DxResPtr<ID3DBlob> m_geometryShaderBlob;
		DxResPtr<ID3DBlob> m_hullShaderBlob;
		DxResPtr<ID3DBlob> m_domainShaderBlob;
		DxResPtr<ID3D11InputLayout> m_inputLayout;
	public:
		Shader() {}
		void Init(const std::wstring& path, const D3D11_INPUT_ELEMENT_DESC* inputLayout, uint32_t numInputLayoutElements, const ShaderEnabling& shaderEnabling)
		{
			ShaderManager::instance().InitShaders(path, shaderEnabling);
			ShaderManager::instance().GetShaderBlobs(path, m_vertexShaderBlob, m_pixelShaderBlob, m_geometryShaderBlob, m_hullShaderBlob, m_domainShaderBlob);
			ShaderManager::instance().GetShaders(path, m_vertexShader, m_pixelShader, m_geometryShader, m_hullShader, m_domainShader);
			m_inputLayout.reset(nullptr);
			if (inputLayout)
			{
				HRESULT result = engine::s_device->CreateInputLayout(inputLayout, numInputLayoutElements, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
				ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
			}
		}

		void InitCompute(const std::wstring& path)
		{
			ShaderManager::instance().InitCompute(path);
			ShaderManager::instance().GetCompute(path, m_computeShader);
		}

		void BindCompute()
		{
			s_deviceContext->CSSetShader(m_computeShader.ptr(), nullptr, 0);
		}

		void Bind()
		{
			s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
			s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
			s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);
			s_deviceContext->GSSetShader(m_geometryShader.ptr(), nullptr, 0);
			s_deviceContext->HSSetShader(m_hullShader.ptr(), nullptr, 0);
			s_deviceContext->DSSetShader(m_domainShader.ptr(), nullptr, 0);
		}
	};
}