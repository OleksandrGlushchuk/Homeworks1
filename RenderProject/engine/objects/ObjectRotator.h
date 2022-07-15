#pragma once
#include "../math/vec3.h"
#include "../math/Quaternion.h"
#include "../d3dobjects/CubeD3D.h"

class IObjectRotator
{
public:
	virtual const Vec3& get_right() const = 0;
	virtual const Vec3& get_top() const = 0;
	virtual const Vec3& get_forward() const = 0;
	virtual void rotate(const Quaternion& q) = 0;
	//virtual void rotate(const Angles& angles, const Vec3& right, const Vec3& top, const Vec3& forward) = 0;
	virtual void rotate(const Angles& angles, Vec3 right, Vec3 top, Vec3 forward) = 0;
};

class CubeRotator : public IObjectRotator
{
public:
	CubeRotator(Cube* _cube) : cube(_cube) {}
	virtual void rotate(const Quaternion& q) override
	{
		cube->Rotate(q);
	}
	virtual void rotate(const Angles& angles, Vec3 right, Vec3 top, Vec3 forward) override
	{
		cube->Rotate(angles, right, top, forward);
	}
	virtual const Vec3& get_right() const override
	{
		return cube->right();
	}
	virtual const Vec3& get_top() const override
	{
		return cube->top();
	}
	virtual const Vec3& get_forward() const override
	{
		return cube->forward();
	}
	Cube* cube;
};