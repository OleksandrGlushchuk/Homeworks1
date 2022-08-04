#include "sphere.h"
bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = -0.5f * b / a;
    else {
        x0 = -0.5f * (b + sqrt(discr)) / a;
        x1 = -0.5f * (b - sqrt(discr)) / a;
    }
    return true;
}

bool math::Sphere::intersection(math::Intersection& nearest, const ray& r) const
{
    Vec3 oc = r.get_origin() - center;
    float a = Vec3::dot(r.get_direction(), r.get_direction());
    float b = 2.0f * Vec3::dot(oc, r.get_direction());
    float c = Vec3::dot(oc, oc) - radius * radius;
    float t1, t2;

    if (solveQuadratic(a, b, c, t1, t2))
    {
        if (t1 < 0)
        {
            t1 = t2;
            if (t1 < 0)
            {
                return false;
            }
        }
        if (nearest.t < t1) return false;

        nearest.t = t1;
        nearest.point = r.point_at_parameter(t1);
        nearest.normal = (nearest.point - center) / radius;
        return true;
    }
    return false;
}
