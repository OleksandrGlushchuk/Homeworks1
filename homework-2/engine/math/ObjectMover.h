#pragma once
#include "vec3.h"
#include "Matr.h"
#include "Cube.h"
#include "sphere.h"
class IObjectMover
{
public:
	virtual void move(const Vec3& offset) = 0;
};

class SphereMover : public IObjectMover
{
public:
	SphereMover(Sphere& _sphere) : sphere(_sphere){}
	virtual void move(const Vec3& offset) override
	{

	}
	Sphere& sphere;
};
class MatrixMover : public IObjectMover
{
public:
	MatrixMover(Matr& _transform) : transform(_transform){}
	virtual void move(const Vec3& offset) override
	{

	}
	Matr& transform;
};

class TransformMover : public IObjectMover
{
public:
	TransformMover(Transform& _transform):transform(_transform){}
	virtual void move(const Vec3& offset) override
	{

	}
	Transform& transform;
};