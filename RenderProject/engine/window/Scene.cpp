#include "Scene.h"

void engine::windows::Scene::Draw()
{
	for (auto& c : cubes)
		c.Draw();

	sky.Draw();
}

void engine::windows::Scene::findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest)
{
	outNearest.reset();

	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i].intersects(_ray, outRef, outNearest);
	}
}

bool engine::windows::Scene::findIntersection(const ray& _ray, IntersectionQuery& query)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	findIntersectionInternal(_ray, ref, query.nearest);

	if (ref.type == ObjRef::IntersectedType::NONE) return false;

	query.nearest.normal.normalize();
	switch (ref.type)
	{
		case ObjRef::IntersectedType::CUBE:
		{
			Cube* cube = static_cast<Cube*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new CubeMover(cube));
				query.rotator.reset(new CubeRotator(cube));
			}
			break;
		}
	}
	return true;
}
