#pragma once
#include "../math/vec3.h"
#include "../d3dobjects/CubeD3D.h"
class IObjectMover
{
public:
	virtual void move(const Vec3& offset) = 0;
	virtual void move_relative(const Vec3& offset) = 0;
	virtual void set_position(const Vec3& pos) = 0;
	virtual const Vec3& get_position() const = 0;
};

class CubeMover : public IObjectMover
{
public:
	CubeMover(Cube* _cube) : cube(_cube) {}
	virtual void move(const Vec3& offset) override
	{
		cube->Translate(offset);
	}
	virtual void move_relative(const Vec3& offset) override
	{
		cube->TranslateRelative(offset);
	}
	virtual void set_position(const Vec3& pos) override
	{
		cube->SetPosition(pos);
	}
	virtual const Vec3& get_position() const override
	{
		return cube->position();
	}



	Cube* cube;
};