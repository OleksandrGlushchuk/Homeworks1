#pragma once
#include "../math/vec3.h"
#include "../math/Quaternion.h"
#include "../objects/Cube.h"

class IObjectRotator
{
public:
	virtual const Vec3& get_right() const = 0;
	virtual const Vec3& get_top() const = 0;
	virtual const Vec3& get_forward() const = 0;
	virtual void rotate(const Quaternion& q) = 0;
	virtual void rotate(const Angles& angles, Vec3 right, Vec3 top, Vec3 forward) = 0;
};

class CubeRotator : public IObjectRotator
{
public:
	CubeRotator(Cube* _cube) : cube(_cube) {}
	virtual void rotate(const Quaternion& q) override
	{
		engine::MeshSystem::instance().Rotate(cube->ID, q);
	}
	virtual void rotate(const Angles& angles, Vec3 right, Vec3 top, Vec3 forward) override
	{
		engine::MeshSystem::instance().Rotate(cube->ID, angles, right, top, forward);
	}
	virtual const Vec3& get_right() const override
	{
		return engine::MeshSystem::instance().Get_Right(cube->ID);
	}
	virtual const Vec3& get_top() const override
	{
		return engine::MeshSystem::instance().Get_Top(cube->ID);
	}
	virtual const Vec3& get_forward() const override
	{
		return engine::MeshSystem::instance().Get_Forward(cube->ID);
	}
	Cube* cube;
};