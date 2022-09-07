#pragma once
#include "vec3.h"
#include "Quaternion.h"

class Transform
{
    Matr<4> transform, transformInv;
    Quaternion rotation;
    Vec3 position;
    Vec3 scale;

public:
    const Vec3& right() 	const { return Vec3(transform[0]).normalized();}
    const Vec3& top() 		const { return Vec3(transform[1]).normalized();}
    const Vec3& forward() 	const { return Vec3(transform[2]).normalized();}
    const Matr<4>& getTransformMatrix() const { return transform; }
    const Matr<4>& getTransformInvMatrix() const { return transformInv; }
    const Vec3& getPosition() const { return position; }
    Transform(){}

    static Transform Identity()
    {
        Transform tr;
        tr.position = Vec3(0, 0, 0);
        tr.scale = Vec3(1, 1, 1);
        tr.rotation = Quaternion::identity();
        tr.transform = tr.transformInv = Matr<4>::identity();
        return tr;
    }

    /*
    * Recieving position, scale and rotation quaternion
    * from _transform. If your transform martix has rotation and scaling this constructor
    * works wrong
    */
    explicit Transform(const Matr<4>& _transform) : transform(_transform)
    {
        position = transform[3];
        scale = Vec3(Vec3(transform[0]).length(), Vec3(transform[1]).length(), Vec3(transform[2]).length());
        float koef = powf(transform.determinant(), 1.f / 3.f);
        rotation.w = sqrt(std::max<float>(0.f, koef + transform[0][0] + transform[1][1] + transform[2][2])) / 2.f;
        rotation.vec.e[0] = sqrt(std::max<float>(0.f, koef + transform[0][0] - transform[1][1] - transform[2][2])) / 2.f;
        rotation.vec.e[1] = sqrt(std::max<float>(0.f, koef - transform[0][0] + transform[1][1] - transform[2][2])) / 2.f;
        rotation.vec.e[2] = sqrt(std::max<float>(0.f, koef - transform[0][0] - transform[1][1] + transform[2][2])) / 2.f;

        rotation.vec.e[0] = _copysign(rotation.vec.e[0], transform[2][1] - transform[1][2]);
        rotation.vec.e[1] = _copysign(rotation.vec.e[1], transform[0][2] - transform[2][0]);
        rotation.vec.e[2] = _copysign(rotation.vec.e[2], transform[1][0] - transform[0][1]);
        transformInv = transform.invert();
    }

    Matr<4> toMat() const
    {
        Matr<4> matr = Matr<4>::identity();
        Matr<3> quat_to_matr = rotation.toMat3();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                matr[i][j] = quat_to_matr[i][j];
            }
        }
        matr[0][0] *= scale.x();
        matr[1][1] *= scale.y();
        matr[2][2] *= scale.z();
        matr[3][0] = position.x();
        matr[3][1] = position.y();
        matr[3][2] = position.z();
        return matr;
    }
    void UpdateMatrices()
    {
        transform = Matr<4>::identity();
        Matr quat_to_matr = rotation.toMat3();
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                transform[i][j] = quat_to_matr[i][j];
            }
        }

        for (int i = 0; i < 3; i++)
        {
            transform[0][i] *= scale.x();
            transform[1][i] *= scale.y();
            transform[2][i] *= scale.z();
        }

        transform[3][0] = position.x();
        transform[3][1] = position.y();
        transform[3][2] = position.z();
        transformInv = transform.invert();
    }
    void Translate(const Vec3& offset)
    {
        position += offset;
        UpdateMatrices();
    }
    void SetPosition(const Vec3& pos)
    {
        position = pos;
        UpdateMatrices();
    }
    void Rotate(const Quaternion& q)
    {
        rotation *= q;
        rotation.normalize();
        UpdateMatrices();
    }
    void Rotate(const Angles& angles, const Vec3 _right, const Vec3 _top, const Vec3 _forward)
    {
        rotation *= Quaternion(angles.roll, _forward);
        rotation *= Quaternion(angles.pitch, _right);
        rotation *= Quaternion(angles.yaw, _top);
        rotation.normalize();
        UpdateMatrices();
    }
    void SetScale(const Vec3& _scale)
    {
        scale = _scale;
        UpdateMatrices();
    }

    void Scale(const Vec3& _scale)
    {
        scale *= _scale;
        UpdateMatrices();
    }
};