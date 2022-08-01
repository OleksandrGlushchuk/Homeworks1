#pragma once
#include "../math/vec3.h"
#include "../objects/Cube.h"
#include "../render/MeshSystem.h"
class IObjectMover
{
public:
	virtual void move(const Vec3& offset) = 0;
	virtual void set_position(const Vec3& pos) = 0;
	virtual const Vec3& get_position() const = 0;
};

class CubeMover : public IObjectMover
{
public:
	CubeMover(Cube* _cube) : cube(_cube) {}
	virtual void move(const Vec3& offset) override
	{
		engine::MeshSystem::instance().Translate(cube->ID, offset);
	}
	virtual void set_position(const Vec3& pos) override
	{
		engine::MeshSystem::instance().SetPosition(cube->ID, pos);
	}
	virtual const Vec3& get_position() const override
	{
		return engine::MeshSystem::instance().Get_Position(cube->ID);
	}
	Cube* cube;
};