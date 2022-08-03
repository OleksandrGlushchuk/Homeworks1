#pragma once

#include "DxRes.hpp"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "MeshSystem.h"
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

		DxResPtr<ID3D11Texture2D> m_depthStencilBuffer;
		
		static Globals *s_instance;

		std::unordered_map<std::string, engine::DxResPtr<ID3D11SamplerState> > m_samplerState;
		engine::DxResPtr<ID3D11SamplerState> m_globalSamplerState;

		Globals() {}
		Globals(const Globals& other) = delete;
		Globals& operator=(const Globals& other) = delete;
		Globals(Globals&& other) noexcept = delete;
		Globals& operator=(Globals&& other) noexcept = delete;

		void initD3D();
		void InitSamplerStates();
		void InitPerFrameBuffer();

		void initDepthStencilResource(UINT width, UINT height);
		void initDepthStencilState();
		void initDepthStencilView();
		void initDepthStencilBuffer()
		{
			initDepthStencilResource(8u,8u);
			initDepthStencilState();
			initDepthStencilView();
		}
	public:
		engine::DxResPtr<ID3D11Buffer> m_perFrameBuffer;

		
		DxResPtr<ID3D11DepthStencilState> m_depthStencilState;
		DxResPtr<ID3D11DepthStencilView> m_depthStencilView;
		static void init();
		static void deinit();
		static Globals& instance();
		void Bind();
		void InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		void SetGlobalSamplerState(const std::string& _globalSamplerStateKey);
		void UpdateDepthStencilBuffer(UINT width, UINT height)
		{
			initDepthStencilResource(width, height);
			initDepthStencilView();
		}
		~Globals();
	};
}