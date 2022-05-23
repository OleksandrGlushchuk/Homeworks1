#pragma once
#include "ray.h" 
#include "Material.h"
#include "Intersection.h"
#include "Transform.h"
class Triangle
{
public:
	Vec3 v0, v1, v2;
	Vec3 normal;
	Triangle(const Vec3 &_v0, const Vec3 &_v1, const Vec3 &_v2, const Vec3 &_normal):v0(_v0),v1(_v1),v2(_v2),normal(_normal){}
	bool intersection(Intersection& nearest, const ray& r) const
	{
		float NdotDir = dot(normal, r.direction);
		if (abs(NdotDir) < 1e-7)
			return false;

		float t = dot(v0 - r.origin, normal) / NdotDir;

		if (t < 0)
			return false;

		if (t > nearest.t)
			return false;

		Vec3 P = r.point_at_parameter(t);
		Vec3 C;

		// edge 0
		Vec3 edge0 = v1 - v0;
		Vec3 vp0 = P - v0;
		C = cross(edge0, vp0);
		if (dot(normal, C) < 0)
			return false;

		// edge 1
		Vec3 edge1 = v2 - v1;
		Vec3 vp1 = P - v1;
		C = cross(edge1, vp1);
		if (dot(normal, C) < 0)
			return false;

		// edge 2
		Vec3 edge2 = v0 - v2;
		Vec3 vp2 = P - v2;
		C = cross(edge2, vp2);
		if (dot(normal, C) < 0)
			return false;

		nearest.point = P;
		nearest.t = t;

		nearest.normal = normal;
		return true;

	}
};

class Cube
{
public:
	Matr matrix = Matr(4);
	Matr matrixInv = Matr(4);
	Transform m_transform;
	std::vector<Triangle> mesh;
	Material mat;
	Cube() {};
	Cube(float size)
	{
		size /= 2.f;
		mesh = std::vector<Triangle>({
		Triangle(Vec3(-size,-size,-size),Vec3(-size,size,-size),Vec3(size,-size,-size),Vec3(0,0,-1)),Triangle(Vec3(size,-size,-size),Vec3(-size,size,-size),Vec3(size,size,-size),Vec3(0,0,-1)),
		Triangle(Vec3(-size,size,-size),Vec3(-size,size,size),Vec3(size,size,-size),Vec3(0,1,0)),Triangle(Vec3(size,size,-size),Vec3(-size,size,size),Vec3(size,size,size),Vec3(0,1,0)),
		Triangle(Vec3(-size,-size,size),Vec3(size,-size,size),Vec3(-size,size,size),Vec3(0,0,1)),Triangle(Vec3(size,-size,size),Vec3(size,size,size),Vec3(-size,size,size),Vec3(0,0,1)),
		Triangle(Vec3(-size,-size,-size),Vec3(size,-size,-size),Vec3(-size,-size,size),Vec3(0,-1,0)),Triangle(Vec3(size,-size,-size),Vec3(size,-size,size),Vec3(-size,-size,size),Vec3(0,-1,0)),
		Triangle(Vec3(-size,-size,-size),Vec3(-size,-size,size),Vec3(-size,size,size),Vec3(-1,0,0)),Triangle(Vec3(-size,-size,-size),Vec3(-size,size,size),Vec3(-size,size,-size),Vec3(-1,0,0)),
		Triangle(Vec3(size,-size,-size),Vec3(size,size,-size),Vec3(size,size,size),Vec3(1,0,0)),Triangle(Vec3(size,-size,-size),Vec3(size,size,size),Vec3(size,-size,size),Vec3(1,0,0))
			});
	}
	Cube(const std::vector<Triangle> &_mesh):mesh(_mesh){}

	bool intersection(Intersection& nearest, const ray& r) const
	{
		ray rr = r;
		rr.direction.mult(matrixInv,0);
		rr.origin.mult(matrixInv,1);
		int k = 0;
		for (int i = 0; i < 12; i++)
		{
			if (mesh[i].intersection(nearest, rr))
			{
				k++;
			}
		}
		if (k > 0)
		{
			nearest.normal.mult(matrix,0);
			nearest.normal.make_unit_vector();
			nearest.point_model = nearest.point;
			nearest.point.mult(matrix,1);
			nearest.mat = mat;
			nearest.cube = this;
			return true;
		}
		return false;
	}
	void Translate(const Matr& m)
	{
		matrix = matrix * m;
		matrixInv = invert(matrix);
	}
	void Translate(float dx, float dy, float dz)
	{
		Matr mt(4);
		mt[3][0] = dx;
		mt[3][1] = dy;
		mt[3][2] = dz;
		matrix = matrix * mt;
		matrixInv = invert(matrix);
	}
	void SetPosition(float x, float y, float z)
	{
		matrix = matrixInv = Matr(4);
		Translate(x, y, z);
	}
};