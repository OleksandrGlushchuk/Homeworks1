#include "d3d.hpp"
#include "globals.hpp"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "MeshSystem.h"

namespace engine
{
	// global pointers to most used D3D11 objects for convenience:
	ID3D11Device5* s_device = nullptr;
	ID3D11DeviceContext4* s_deviceContext = nullptr;
	IDXGIFactory5* s_factory = nullptr;

	void init()
	{
		Globals::init();

		TextureManager::init();
		ShaderManager::init();
		ModelManager::init();
		MeshSystem::init();
		
	}
	void deinit()
	{
		MeshSystem::deinit();
		ModelManager::deinit();
		ShaderManager::deinit();
		TextureManager::deinit();

		Globals::deinit();
	}
}