#include "Sky.h"

void Sky::InitTextures()
{
	engine::TextureManager::instance().LoadTexture(L"source/textures/skymap.dds");
}

void Sky::InitShaders()
{
	engine::ShaderManager::instance().InitShaders(L"source/shaders/sky.hlsl");
	engine::ShaderManager::instance().GetShaderBlobs(L"source/shaders/sky.hlsl", Sky::s_vertexShaderBlob, Sky::s_pixelShaderBlob);
	engine::ShaderManager::instance().GetShaders(L"source/shaders/sky.hlsl", Sky::s_vertexShader, Sky::s_pixelShader);
}

void Sky::Init()
{
	Sky::InitTextures();
	Sky::InitShaders();
}

void Sky::Draw()
{
	engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	engine::s_deviceContext->IASetInputLayout(nullptr);
	engine::s_deviceContext->VSSetShader(s_vertexShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShader(s_pixelShader.ptr(), nullptr, 0);
	engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

	engine::s_deviceContext->Draw(3, 0);
}
