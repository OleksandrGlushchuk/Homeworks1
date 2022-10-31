#include "globals.hpp"
#include <Windows.h>
#include "SamplerManager.h"
// Say NVidia or AMD driver to prefer a dedicated GPU instead of an integrated.
// This has effect on laptops.
extern "C"
{
	_declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	_declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
	Globals *Globals::s_instance = nullptr;
	void Globals::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new Globals;
		s_instance->initD3D();
		s_instance->m_perFrameBuffer.Init(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	}
	void Globals::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}
	Globals& Globals::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
	void Globals::initD3D()
	{
		HRESULT result;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_factory.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDXGIFactory");

		result = m_factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_factory5.reset());
		ALWAYS_ASSERT(result >= 0 && "Query IDXGIFactory5");

		{
			uint32_t index = 0;
			IDXGIAdapter1* adapter;
			while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				//LOG << "GPU #" << index << desc.Description;
			}
		}

		// Init D3D Device & Context

		const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
			&featureLevelRequested, 1, D3D11_SDK_VERSION, m_device.reset(), &featureLevelInitialized, m_devcon.reset());
		ALWAYS_ASSERT(result >= 0 && "D3D11CreateDevice");
		ALWAYS_ASSERT(featureLevelRequested == featureLevelInitialized && "D3D_FEATURE_LEVEL_11_0");

		result = m_device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11Device5");

		result = m_devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devcon4.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11DeviceContext4");

		result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devdebug.reset());
		ALWAYS_ASSERT(result >= 0 && "Query ID3D11Debug");

		// Write global pointers

		s_factory = m_factory5.ptr();
		s_device = m_device5.ptr();
		s_deviceContext = m_devcon4.ptr();
	}

	void Globals::Bind()
	{
		m_perFrameBuffer.BindVS(0);
		m_perFrameBuffer.BindPS(0);
		m_perFrameBuffer.BindGS(0);
		engine::s_deviceContext->PSSetSamplers(0, 1, &SamplerManager::instance().GetGlobalSamplerState().ptr());
		engine::s_deviceContext->PSSetSamplers(1, 1, &SamplerManager::instance().GetSamplerState("ss_mmlmp_clamp").ptr());
		engine::s_deviceContext->PSSetSamplers(2, 1, &SamplerManager::instance().GetSamplerState("ss_mmmp").ptr());
		engine::s_deviceContext->PSSetSamplers(3, 1, &SamplerManager::instance().GetSamplerState("ss_cmmlmp").ptr());
		engine::s_deviceContext->PSSetSamplers(4, 1, &SamplerManager::instance().GetSamplerState("ss_masked").ptr());
	}

	void Globals::UpdatePerFrameBuffer(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, 
		uint32_t sampleCount, uint32_t _screenWidth, uint32_t _screenHeight)
	{
		m_perFrameBuffer.Update(PerFrameBuffer(camera, currentTime, LightSystem::instance().getPointLights(),
			LightSystem::instance().getDirectionalLights(), sampleCount, _screenWidth, _screenHeight));
	}

	Globals::~Globals()
	{
		s_factory = nullptr;
		s_device = nullptr;
		s_deviceContext = nullptr;
	}
}