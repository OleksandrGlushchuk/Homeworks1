#include "TransformSystem.h"

namespace engine
{
	TransformSystem* TransformSystem::s_instance = nullptr;
	void TransformSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new TransformSystem;
	}

	void TransformSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	TransformSystem& TransformSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
}