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
		DxResPtr<ID3DBlob> m_vertexShaderBlob;
		DxResPtr<ID3DBlob> m_pixelShaderBlob;
		DxResPtr<ID3DBlob> m_geometryShaderBlob;
		DxResPtr<ID3D11InputLayout> m_inputLayout;
	public:
		Shader() {}
		void Init(const std::wstring& path, const D3D11_INPUT_ELEMENT_DESC* inputLayout, uint32_t numInputLayoutElements, const ShaderEnabling& shaderEnabling)
		{
			ShaderManager::instance().InitShaders(path, shaderEnabling);
			ShaderManager::instance().GetShaderBlobs(path, m_vertexShaderBlob, m_pixelShaderBlob, m_geometryShaderBlob);
			ShaderManager::instance().GetShaders(path, m_vertexShader, m_pixelShader, m_geometryShader);
			m_inputLayout.reset(nullptr);
			if (inputLayout)
			{
				HRESULT result = engine::s_device->CreateInputLayout(inputLayout, numInputLayoutElements, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
				ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
			}
		}
		void Bind()
		{
			s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
			s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
			s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);
			s_deviceContext->GSSetShader(m_geometryShader.ptr(), nullptr, 0);
		}
	};
}