#include "d3d.hpp"
#include "singletones/globals.hpp"
#include "singletones/TextureManager.h"
#include "singletones/ShaderManager.h"
#include "singletones/ModelManager.h"
#include "singletones/MeshSystem.h"
#include "singletones/VegetationSystem.h"
#include "singletones/LightSystem.h"
#include "singletones/TransformSystem.h"
#include "singletones/SamplerManager.h"
#include "singletones/DepthStencilManager.h"
#include "singletones/ShadowManager.h"
#include "singletones/BlendStateManager.h"
#include "singletones/RasteriserStateManager.h"
#include "singletones/ParticleSystem.h"


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
		ShadowManager::init();
		LightSystem::init();
		TransformSystem::init();
		SamplerManager::init();
		DepthStencilManager::init();
		BlendStateManager::init();
		RasteriserStateManager::init();
		ParticleSystem::init();
		MeshSystem::init();
		VegetationSystem::init();
	}
	void deinit()
	{
		VegetationSystem::deinit();
		MeshSystem::deinit();
		ParticleSystem::deinit();
		RasteriserStateManager::deinit();
		BlendStateManager::deinit();
		DepthStencilManager::deinit();
		SamplerManager::deinit();
		TransformSystem::deinit();
		LightSystem::deinit();
		ShadowManager::deinit();
		ModelManager::deinit();
		ShaderManager::deinit();
		TextureManager::deinit();

		Globals::deinit();
	}
}