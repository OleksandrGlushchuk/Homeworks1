#include "globals.hpp"
#include <Windows.h>
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
		s_instance->initDepthStencilBuffer();
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

			InitSamplerState(samplerDesc, "ss_a");
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

			InitSamplerState(samplerDesc, "ss_mmmp");
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

			InitSamplerState(samplerDesc, "ss_mpmlmp");
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

			InitSamplerState(samplerDesc, "ss_mmlmp");
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

			InitSamplerState(samplerDesc, "ss_mmml");
			SetGlobalSamplerState("ss_mmml");
		}
	}

	void Globals::InitSamplerState(D3D11_SAMPLER_DESC& samplerDesc, const std::string& samplerStateKey)
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

	void Globals::initDepthStencilResource(UINT width, UINT height)
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;

		depthBufferDesc.Width = std::max<UINT>(width, 8u);
		depthBufferDesc.Height = std::max<UINT>(height, 8u);
		depthBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT result = engine::s_device->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
	}

	void Globals::initDepthStencilState()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;

		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

		HRESULT result = engine::s_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilState");
	}

	void Globals::initDepthStencilView()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		HRESULT result = engine::s_device->CreateDepthStencilView(m_depthStencilBuffer.ptr(), &depthStencilViewDesc, m_depthStencilView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilView");
	}

	void Globals::InitPerFrameBuffer()
	{
		D3D11_BUFFER_DESC perFrameBufferDesc;
		perFrameBufferDesc.ByteWidth = sizeof(PerFrameBuffer);
		perFrameBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		perFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		perFrameBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		perFrameBufferDesc.MiscFlags = 0;
		perFrameBufferDesc.StructureByteStride = 0;

		HRESULT result = engine::s_device->CreateBuffer(&perFrameBufferDesc, nullptr, m_perFrameBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	void Globals::Bind()
	{
		engine::s_deviceContext->VSSetConstantBuffers(0, 1, &m_perFrameBuffer.ptr());
		engine::s_deviceContext->PSSetSamplers(0, 1, &m_globalSamplerState.ptr());
	}

	Globals::~Globals()
	{
		s_factory = nullptr;
		s_device = nullptr;
		s_deviceContext = nullptr;
	}
}