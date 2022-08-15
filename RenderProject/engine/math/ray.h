#pragma once
#include "vec3.h"
#include <limits>
#include "Box.h"
#include "MeshIntersection.h"
#include "Triangle.h"

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
    bool intersect(float t, const engine::Box& box) const
    {
        bool result = false;
        engine::MeshIntersection out_meshIntersection;
        out_meshIntersection.reset(0);
        out_meshIntersection.t = t;
        Vec3 c1 = Vec3(box.min.e[0], box.max.e[1], box.min.e[2]);
        Vec3 c2 = Vec3(box.max.e[0], box.max.e[1], box.min.e[2]);
        Vec3 c3 = Vec3(box.max.e[0], box.min.e[1], box.min.e[2]);

        Vec3 c4 = Vec3(box.max.e[0], box.min.e[1], box.max.e[2]);
        Vec3 c5 = Vec3(box.min.e[0], box.min.e[1], box.max.e[2]);
        Vec3 c6 = Vec3(box.min.e[0], box.max.e[1], box.max.e[2]);

        //Face
        result |= intersect(out_meshIntersection, box.min, c1, c2);
        result |= intersect(out_meshIntersection, c2, c3, box.min);

        //Up
        result |= intersect(out_meshIntersection, c1, c6, box.max);
        result |= intersect(out_meshIntersection, box.max, c2, c1);

        //Back
        result |= intersect(out_meshIntersection, c4, box.max, c6);
        result |= intersect(out_meshIntersection, c6, c5, c4);

        //Down
        result |= intersect(out_meshIntersection, c5, box.min, c3);
        result |= intersect(out_meshIntersection, c3, c4, c5);

        //Left
        result |= intersect(out_meshIntersection, c5, c6, c1);
        result |= intersect(out_meshIntersection, c1, box.min, c5);

        //Right
        result |= intersect(out_meshIntersection, c3, c2, box.max);
        result |= intersect(out_meshIntersection, box.max, c4, c3);

        return result;
       // return box.contains(this->point_at_parameter(t));
    }
    bool intersect(engine::MeshIntersection& out_meshIntersection, const Vec3& V0, const Vec3& V1, const Vec3& V2) const
    {
        math::Triangle tr(V0, V1, V2, Vec3::cross(V1 - V0, V2 - V0));
        return tr.intersection(out_meshIntersection,*this);
    }
};