#pragma once
#include "../math/vec3.h"
#include "../d3dobjects/CubeD3D.h"
//#include "../math/Light.h"
//#include "Cube.h"
//#include "sphere.h"
class IObjectMover
{
public:
	virtual void move(const Vec3& offset) = 0;
};

//class SphereMover : public IObjectMover
//{
//public:
//	SphereMover(Sphere* _sphere) : sphere(_sphere) {}
//	virtual void move(const Vec3& offset) override
//	{
//		sphere->center += offset;
//	}
//	Sphere* sphere;
//};
//
//class Sphere_Point_Light_Mover : public IObjectMover
//{
//public:
//	Sphere_Point_Light_Mover(Sphere_Point_Light* _spl):spl(_spl){}
//	virtual void move(const Vec3& offset) override
//	{
//		spl->sphere.center += offset;
//		spl->light.pos += offset;
//	}
//	Sphere_Point_Light* spl;
//};
//
//class Sphere_Spot_Light_Mover : public IObjectMover
//{
//public:
//	Sphere_Spot_Light_Mover(Sphere_Spot_Light* _ssl) :ssl(_ssl) {}
//	virtual void move(const Vec3& offset) override
//	{
//		ssl->sphere.center += offset;
//		ssl->light.pos += offset;
//	}
//	Sphere_Spot_Light* ssl;
//};

class CubeMover : public IObjectMover
{
public:
	CubeMover(Cube* _cube) : cube(_cube) {}
	virtual void move(const Vec3& offset) override
	{
		cube->Translate(offset);
	}
	Cube* cube;
};