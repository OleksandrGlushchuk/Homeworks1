#include "sphere.h"

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = -0.5 * b / a;
    else {
        x0 = -0.5 * (b + sqrt(discr)) / a;
        x1 = -0.5 * (b - sqrt(discr)) / a;
    }
    return true;
}

bool sphere::hit(const ray& r, hit_record& record) const {

    vec3 oc = r.get_origin() - center;
    float a = dot(r.get_direction(), r.get_direction());
    float b = 2.0 * dot(oc, r.get_direction());
    float c = dot(oc, oc) - radius * radius;
    float t1, t2;

    if (solveQuadratic(a, b, c, t1, t2))
    {
        if (t1 < 0)
        {
            t1 = t2;
            if (t1 < 0)
            {
                record.t = -999.999;
                record.hit_point = vec3(-999.999, -999.999, -999.999);
                record.normal = record.hit_point;
                return false;
            }
                
        }
        record.t = t1;
        record.hit_point = r.point_at_parameter(t1);
        record.normal = (record.hit_point - center) / radius;
        return true;
    }
    record.t = -999.999;
    record.hit_point = vec3(-999.999, -999.999, -999.999);
    record.normal = record.hit_point;
    return false;
}