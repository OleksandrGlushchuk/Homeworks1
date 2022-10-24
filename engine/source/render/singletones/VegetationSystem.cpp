#include "VegetationSystem.h"

namespace engine
{
	VegetationSystem* VegetationSystem::s_instance;
	void VegetationSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new VegetationSystem;
		s_instance->grassField.Init();
	}

	void VegetationSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	VegetationSystem& VegetationSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
}