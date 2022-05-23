#pragma once
#include "ray.h"
#include "camera.h"
#include "Intersection.h"
#include "Material.h"
class Sphere {
public:
    Sphere() {};
    Sphere(Vec3 cen, float r) : center(cen), radius(r){};

    bool intersection(Intersection& nearest, const ray& r) const;
    Vec3 center;
    float radius;
    Material mat;
};