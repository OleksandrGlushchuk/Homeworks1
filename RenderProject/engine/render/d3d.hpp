#pragma once

#include "../include/win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


#define BREAK __debugbreak();
#define ALWAYS_ASSERT(expression, ...)\
if (!(expression))\
{\
	BREAK;\
	std::abort();\
}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif

namespace engine
{
	void init();
	void deinit();
	// global pointers to most used D3D11 objects for convenience:
	extern ID3D11Device5* s_device;
	extern ID3D11DeviceContext4* s_deviceContext;
	extern IDXGIFactory5* s_factory;
}

#include "../include/win_undef.hpp"