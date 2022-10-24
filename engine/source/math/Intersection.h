#pragma once
#include "vec3.h"
#include <cmath>
#include <limits>
namespace math
{
	class Intersection
	{
	public:
		float t;
		Vec3 point;
		Vec3 normal;
		void reset() { t = std::numeric_limits<float>::infinity(); }
		bool exists() const { return std::isfinite(t); }
		static Intersection infinite() { Intersection inf; inf.reset(); return inf; }
	};
}