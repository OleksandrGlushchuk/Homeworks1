#pragma once

#include "DxRes.hpp"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "PerFrameBuffer.h"

namespace engine
{
	class Globals // a singletone for accessing global rendering resources
	{
		DxResPtr<IDXGIFactory> m_factory;
		DxResPtr<IDXGIFactory5> m_factory5;
		DxResPtr<ID3D11Device> m_device;
		DxResPtr<ID3D11Device5> m_device5;
		DxResPtr<ID3D11DeviceContext> m_devcon;
		DxResPtr<ID3D11DeviceContext4> m_devcon4;
		DxResPtr<ID3D11Debug> m_devdebug;
		static Globals *s_instance;
		Globals() {}
	public:
		engine::DxResPtr<ID3D11Buffer> m_perFrameBuffer;
		std::map<std::string, engine::DxResPtr<ID3D11SamplerState> > m_samplerState;
		engine::DxResPtr<ID3D11SamplerState> m_globalSamplerState;

		static void init();
		static void deinit();
		static Globals& instance();

		void initD3D();
		void InitSamplerStates();
		void InitTextures();
		void InitShaders();

		void bind();
		~Globals();
	};
}