#pragma once
#include "ray.h"
#include "Intersection.h"
namespace math
{
    class Sphere {
    public:
        Sphere() {};
        Sphere(const Vec3& cen, float r) : center(cen), radius(r) {};
        bool intersection(math::Intersection& nearest, const ray& r) const;
        Vec3 center;
        float radius;
    };
}