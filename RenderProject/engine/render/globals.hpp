#pragma once

#include "DxRes.hpp"
#include "TextureManager.h"

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
		static Globals m_instance;
		Globals() {}
	public:
		static Globals& instance();
		void initD3D();
		~Globals();
	};
}