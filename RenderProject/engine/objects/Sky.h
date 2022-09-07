#pragma once
#include "../render/globals.hpp"
#include "../render/Shader.h"

class Sky
{
	struct ReflectionMipsBuffer
	{
		ReflectionMipsBuffer() {}
		ReflectionMipsBuffer(float mipsCount) : reflectionMips(mipsCount) {}
		float reflectionMips;
		Vec3 paddingRM;
	};
		
	ConstantBuffer<ReflectionMipsBuffer> m_reflectionMipsBuffer;
	Texture m_irradiance;
	Texture m_reflectance;
	Texture m_reflection;
	Shader m_shader;

public:
	Texture m_texture;
	Sky(){}
	void Init()
	{
		m_shader.Init(L"source/shaders/sky.hlsl", nullptr, 0, true, false);
	}
	void Draw()
	{
		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_shader.Bind();
		m_texture.Bind(0);
		engine::s_deviceContext->Draw(3, 0);
	}

	void SetEnvironment(const std::wstring& irradiance, const std::wstring& reflectance, const std::wstring& reflection)
	{
		m_irradiance.Load(irradiance); m_reflectance.Load(reflectance); m_reflection.Load(reflection);
		auto cData = ReflectionMipsBuffer(m_reflection.GetDesc().TextureCube.MipLevels);
		m_reflectionMipsBuffer.Init(D3D11_USAGE_DEFAULT, 0, &cData);
	}
	void BindEnvironment()
	{
		m_irradiance.Bind(10);
		m_reflectance.Bind(11);
		m_reflection.Bind(12);
		m_reflectionMipsBuffer.BindPS(10);
	}
};