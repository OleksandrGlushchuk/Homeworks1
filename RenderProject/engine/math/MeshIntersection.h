#pragma once
#include "vec3.h"
#include <limits>
#include <cmath>
#include <cstdint>

namespace engine
{
	struct MeshIntersection
	{
		Vec3 pos;
		Vec3 normal;
		float near;
		float t;
		uint32_t triangle;

		constexpr void reset(float near, float far = std::numeric_limits<float>::infinity())
		{
			this->near = near;
			t = far;
		}
		bool valid() const { return std::isfinite(t); }
	};
}