#pragma once
#include "../DxRes.hpp"
#include "../SolidVector.h"
#include "../../math/Transform.h"
namespace engine
{
	class TransformSystem
	{
		
		static TransformSystem* s_instance;
		TransformSystem() {}
		TransformSystem(const TransformSystem& other) = delete;
		TransformSystem& operator=(const TransformSystem& other) = delete;
		TransformSystem(TransformSystem&& other) noexcept = delete;
		TransformSystem& operator=(TransformSystem&& other) noexcept = delete;
	public:
		static void init();
		static void deinit();
		static TransformSystem& instance();
		SolidVector<Transform> m_transforms;
	};
}