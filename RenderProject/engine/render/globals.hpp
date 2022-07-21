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

		std::map<std::string, engine::DxResPtr<ID3D11SamplerState> > m_samplerState;
		engine::DxResPtr<ID3D11SamplerState> m_globalSamplerState;

		Globals() {}
		void initD3D();
		void InitSamplerStates();
		void InitPerFrameBuffer();
	public:
		engine::DxResPtr<ID3D11Buffer> m_perFrameBuffer;

		static void init();
		static void deinit();
		static Globals& instance();
		void bind();
		void InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		void SetGlobalSamplerState(const std::string& _globalSamplerStateKey);
		~Globals();
	};
}