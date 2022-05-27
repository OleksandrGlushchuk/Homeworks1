#pragma once
#include "ray.h" 
#include "Intersection.h"
#include "Transform.h"
#include "Triangle.h"
#include <vector>
namespace math
{
	class Cube
	{
	public:
		Transform m_transform;
		static std::vector<math::Triangle> mesh;
		const Vec3& right() 	const { return m_transform.right(); }
		const Vec3& top() 		const { return m_transform.top(); }
		const Vec3& forward() 	const { return m_transform.forward(); }

		Cube() {};
		Cube(float size);
		Cube(const Vec3& scale);

		bool intersection(math::Intersection& nearest, const ray& r) const;

		void Rotate(const Quaternion& q);

		void Translate(const Vec3& dv);

		void SetPosition(const Vec3& pos);
	};
}