#include "LightSystem.h"

namespace engine
{
	LightSystem* LightSystem::s_instance = nullptr;
	void LightSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new LightSystem;
	}

	void LightSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	LightSystem& LightSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
}