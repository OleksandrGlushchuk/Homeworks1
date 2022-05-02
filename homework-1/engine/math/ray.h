#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray
{
public:
    ray() {}
    ray(const vec3& _origin, const vec3& _direction) { origin = _origin; direction = _direction; }
    vec3 get_origin() const { return origin; }
    vec3 get_direction() const { return direction; }
    vec3 point_at_parameter(float t) const { return origin + t * direction; }
    vec3 origin;
    vec3 direction;
};

#endif