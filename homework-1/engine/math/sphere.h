#pragma once
#include "ray.h"
#include <limits>
class hit_record
{
public:
    float t;
    vec3 hit_point;
    vec3 normal;
    bool isValid() const
    {
        return t < std::numeric_limits<float>::infinity();
    }
};

class sphere {
public:
    sphere() = default;
    sphere(vec3 cen, float r) : center(cen), radius(r){};
    bool hit(const ray& r, hit_record &record) const;
    vec3 center;
    float radius;
};