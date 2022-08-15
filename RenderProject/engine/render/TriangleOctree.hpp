#pragma once
#include "../math/Box.h"
#include <cmath>
#include <array>
#include <memory>
#include "../math/ray.h"
#include <vector>

namespace engine
{	
	struct Mesh;
	class TriangleOctree
	{
	public:
		const static int PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(const Mesh& mesh);

		bool intersect(const ray& ray, MeshIntersection& nearest) const;

	protected:
		const Mesh* m_mesh = nullptr;
		std::vector<uint32_t> m_triangles;

		Box m_box;
		Box m_initialBox;

		std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

		void initialize(const Mesh& mesh, const Box& parentBox, const Vec3& parentCenter, int octetIndex);

		bool addTriangle(uint32_t triangleIndex, const Vec3& V1, const Vec3& V2, const Vec3& V3, const Vec3& center);

		bool intersectInternal(const ray& ray, MeshIntersection& nearest) const;
	};
}