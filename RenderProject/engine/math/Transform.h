#pragma once
#include "vec3.h"
#include "Quaternion.h"

struct Transform
{
    Matr<4> transform = Matr<4>::identity(),
            transformInv = Matr<4>::identity();
    Quaternion rotation = Quaternion::identity();
    Vec3 position;
    Vec3 scale;
    const Vec3& right() 	const { return Vec3(transform[0]) / Vec3::length(transform[0]); }
    const Vec3& top() 		const { return Vec3(transform[1]) / Vec3::length(transform[1]); }
    const Vec3& forward() 	const { return Vec3(transform[2]) / Vec3::length(transform[2]); }
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
};