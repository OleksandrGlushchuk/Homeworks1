#include "globals.hpp"
#include <Windows.h>
#include "../math/camera.h"
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
		s_instance->InitSamplerStates();
		s_instance->InitPerFrameBuffer();
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

	void Globals::InitSamplerStates()
	{
		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;

			CreateSamplerState(samplerDesc, "ss_a");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;

			CreateSamplerState(samplerDesc, "ss_mmmp");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;

			CreateSamplerState(samplerDesc, "ss_mpmlmp");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;

			CreateSamplerState(samplerDesc, "ss_mmlmp");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;

			CreateSamplerState(samplerDesc, "ss_mmml");
			SetGlobalSamplerState("ss_mmml");
		}
	}

	void Globals::CreateSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey)
	{
		HRESULT result = engine::s_device->CreateSamplerState(&samplerDesc, m_samplerState[samplerStateKey].reset());
		ALWAYS_ASSERT(result >= 0 && "CreateSamplerState");
	}

	void Globals::SetGlobalSamplerState(const std::string& _globalSamplerStateKey)
	{
		std::unordered_map< std::string, engine::DxResPtr<ID3D11SamplerState> >::iterator result;
		ALWAYS_ASSERT((result = m_samplerState.find(_globalSamplerStateKey)) != m_samplerState.end() && "Bad globalSamplerStateKey");
		engine::Globals::instance().m_globalSamplerState = result->second;
	}

	void Globals::InitPerFrameBuffer()
	{
		m_perFrameBuffer.Init(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	}

	void Globals::Bind()
	{
		m_perFrameBuffer.BindVS(0);
		m_perFrameBuffer.BindPS(0);
		engine::s_deviceContext->PSSetSamplers(0, 1, &m_globalSamplerState.ptr());
		engine::s_deviceContext->PSSetSamplers(1, 1, &m_samplerState["ss_mmmp"].ptr());
	}

	void Globals::UpdatePerFrameBuffer(const Camera& camera)
	{
		auto mapping = m_perFrameBuffer.Map();
		*(PerFrameBuffer*)mapping.pData = PerFrameBuffer(camera.m_viewProj, camera.BottomLeft, camera.BR_M_BL, camera.TL_M_BL, 
			camera.position(), engine::LightSystem::instance().pointLight);
		m_perFrameBuffer.Unmap();
	}

	Globals::~Globals()
	{
		s_factory = nullptr;
		s_device = nullptr;
		s_deviceContext = nullptr;
	}
}