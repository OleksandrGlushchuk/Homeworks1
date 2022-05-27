#ifndef RAYH
#define RAYH
#include "vec3.h"
#include <limits>
class hit_record
{
public:
    float t = std::numeric_limits<float>::infinity();
    float distance_to_camera = std::numeric_limits<float>::infinity();
    Vec3 hit_point = Vec3(0, 0, 0);
    Vec3 normal = Vec3(0, 0, 0);
    bool isValid() const
    {
        return t < std::numeric_limits<float>::infinity();
    }
};

class ray
{
public:
    ray() {}
    ray(const Vec3& _origin, const Vec3& _direction) { origin = _origin; direction = _direction; }
    Vec3 get_origin() const { return origin; }
    Vec3 get_direction() const { return direction; }
    Vec3 point_at_parameter(float t) const { return origin + t * direction; }
    Vec3 origin;
    Vec3 direction;
};

#endif