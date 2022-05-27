#include "Cube.h"
std::vector<math::Triangle> math::Cube::mesh({
		math::Triangle(Vec3(-1,-1,-1),Vec3(-1,1,-1),Vec3(1,-1,-1),Vec3(0,0,-1)),	math::Triangle(Vec3(1,-1,-1),Vec3(-1,1,-1),Vec3(1,1,-1),Vec3(0,0,-1)),
		math::Triangle(Vec3(-1,1,-1),	Vec3(-1,1,1),Vec3(1,1,-1),Vec3(0,1,0)),		math::Triangle(Vec3(1,1,-1),Vec3(-1,1,1),Vec3(1,1,1),Vec3(0,1,0)),
		math::Triangle(Vec3(-1,-1,1),Vec3(1,-1,1),Vec3(-1,1,1),Vec3(0,0,1)),		math::Triangle(Vec3(1,-1,1),Vec3(1,1,1),Vec3(-1,1,1),Vec3(0,0,1)),
		math::Triangle(Vec3(-1,-1,-1),Vec3(1,-1,-1),Vec3(-1,-1,1),Vec3(0,-1,0)),	math::Triangle(Vec3(1,-1,-1),Vec3(1,-1,1),Vec3(-1,-1,1),Vec3(0,-1,0)),
		math::Triangle(Vec3(-1,-1,-1),Vec3(-1,-1,1),Vec3(-1,1,1),Vec3(-1,0,0)),		math::Triangle(Vec3(-1,-1,-1),Vec3(-1,1,1),Vec3(-1,1,-1),Vec3(-1,0,0)),
		math::Triangle(Vec3(1,-1,-1),Vec3(1,1,-1),Vec3(1,1,1),Vec3(1,0,0)),			math::Triangle(Vec3(1,-1,-1),Vec3(1,1,1),Vec3(1,-1,1),Vec3(1,0,0))
	});

math::Cube::Cube(float size)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = Vec3(size / 2.f, size / 2.f, size / 2.f);
	m_transform.UpdateMatrices();
}

math::Cube::Cube(const Vec3& scale)
{
	m_transform.position = Vec3(0, 0, 0);
	m_transform.scale = scale / 2.f;
	m_transform.UpdateMatrices();
}

bool math::Cube::intersection(math::Intersection& nearest, const ray& r) const
{
	ray rr = r;
	rr.direction.mult(m_transform.transformInv, 0);
	rr.origin.mult(m_transform.transformInv, 1);
	int k = 0;
	for (int i = 0; i < 12; i++)
	{
		if (Cube::mesh[i].intersection(nearest, rr))
		{
			k++;
		}
	}
	if (k > 0)
	{
		nearest.normal.mult(m_transform.transform, 0);
		make_unit_vector(nearest.normal);
		nearest.point_without_translation = (nearest.point * m_transform.scale).mult(m_transform.rotation.toMat3(), 1);
		nearest.point.mult(m_transform.transform, 1);
		return true;
	}
	return false;
}

void math::Cube::Rotate(const Quaternion& q)
{
	m_transform.rotation *= q;
	m_transform.rotation.normalize();
	m_transform.UpdateMatrices();
}

void math::Cube::Translate(const Vec3& dv)
{
	m_transform.position += dv;
	m_transform.UpdateMatrices();
}

void math::Cube::SetPosition(const Vec3& pos)
{
	m_transform.position = pos;
	m_transform.UpdateMatrices();
}
