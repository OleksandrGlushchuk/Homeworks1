#pragma once
#include "vec3.h"
#include "Quaternion.h"

struct Transform
{
    Matr transform = Matr(4);
    Matr transformInv = Matr(4);
    Quaternion rotation;
    Vec3 position;
    Vec3 scale;
    const Vec3& right() 	const { return transform[0]/length(transform[0]); }
    const Vec3& top() 		const { return transform[1]/length(transform[1]); }
    const Vec3& forward() 	const { return transform[2]/length(transform[2]); }
    Matr toMat() const
    {
        Matr matr(4);
        Matr quat_to_matr = rotation.toMat3();
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
        transform = Matr(4);
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
        transformInv = invert(transform);
    }
};