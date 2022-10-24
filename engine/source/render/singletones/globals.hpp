#pragma once
#include "../DxRes.hpp"
#include "../ConstantBuffer.h"
#include "../PerFrameBuffer.h"
#include <chrono>
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

		Globals() {}
		Globals(const Globals& other) = delete;
		Globals& operator=(const Globals& other) = delete;
		Globals(Globals&& other) noexcept = delete;
		Globals& operator=(Globals&& other) noexcept = delete;

		void initD3D();
	public:
		static void init();
		static void deinit();
		static Globals& instance();
		void Bind();
		void UpdatePerFrameBuffer(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, 
			uint32_t sampleCount, uint32_t _screenWidth, uint32_t _screenHeight);
		~Globals();
	};
}