#pragma once

#include "DxRes.hpp"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "MeshSystem.h"
#include "ConstantBuffer.h"
#include "PerFrameBuffer.h"
#include "LightSystem.h"

class Camera;
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

		ConstantBuffer<PerFrameBuffer> m_perFrameBuffer;
		
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
	public:
		static void init();
		static void deinit();
		static Globals& instance();
		void Bind();
		void CreateSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey);
		void SetGlobalSamplerState(const std::string& _globalSamplerStateKey);

		void UpdatePerFrameBuffer(const Camera& camera);
		~Globals();
	};
}