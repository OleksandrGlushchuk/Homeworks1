#pragma once
#include "ShaderManager.h"
#include <string>

class Shader
{
	engine::DxResPtr<ID3D11VertexShader> m_vertexShader;
	engine::DxResPtr<ID3D11PixelShader> m_pixelShader;
	engine::DxResPtr<ID3D11GeometryShader> m_geometryShader;
	engine::DxResPtr<ID3DBlob> m_vertexShaderBlob;
	engine::DxResPtr<ID3DBlob> m_pixelShaderBlob;
	engine::DxResPtr<ID3DBlob> m_geometryShaderBlob;
	engine::DxResPtr<ID3D11InputLayout> m_inputLayout;
public:
	Shader(){}
	void Init(const std::wstring& path, const D3D11_INPUT_ELEMENT_DESC *inputLayout, uint32_t numInputLayoutElements, bool hasPixelShader, bool hasGeometryShader)
	{
		engine::ShaderManager::instance().InitShaders(path, hasPixelShader, hasGeometryShader);
		engine::ShaderManager::instance().GetShaderBlobs(path, m_vertexShaderBlob, m_pixelShaderBlob, m_geometryShaderBlob);
		engine::ShaderManager::instance().GetShaders(path, m_vertexShader, m_pixelShader, m_geometryShader);
		m_inputLayout.reset(nullptr);
		if (inputLayout)
		{
			HRESULT result = engine::s_device->CreateInputLayout(inputLayout, numInputLayoutElements, m_vertexShaderBlob->GetBufferPointer(), m_vertexShaderBlob->GetBufferSize(), m_inputLayout.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateInputLayout");
		}
	}
	void Bind()
	{
		engine::s_deviceContext->IASetInputLayout(m_inputLayout.ptr());
		engine::s_deviceContext->VSSetShader(m_vertexShader.ptr(), nullptr, 0);
		engine::s_deviceContext->PSSetShader(m_pixelShader.ptr(), nullptr, 0);
		engine::s_deviceContext->GSSetShader(m_geometryShader.ptr(), nullptr, 0);
	}
};