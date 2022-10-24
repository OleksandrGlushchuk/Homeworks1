#pragma once
#include "vec3.h"
#include <numeric>

namespace engine
{
	struct Box
	{
		Vec3 min;
		Vec3 max;

		static constexpr float Inf = std::numeric_limits<float>::infinity();
		static const Box empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
		static const Box unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

		Vec3 size() const { return max - min; } 
		Vec3 center() const { return (min + max) / 2.f; }
		float radius() const { return size().length() / 2.f; }

		void reset()
		{
			constexpr float maxf = std::numeric_limits<float>::max();
			min = { maxf , maxf , maxf };
			max = -min;
		}

		void expand(const Box& other)
		{
			min = other.min;
			max = other.max;
		}

		void expand(const Vec3& point)
		{
			min = point;
			max = point;
		}

		bool contains(const Vec3& P) const
		{
			return
				min[0] <= P[0] && P[0] <= max[0] &&
				min[1] <= P[1] && P[1] <= max[1] &&
				min[2] <= P[2] && P[2] <= max[2];
		}
	};
}