#pragma once
#include "vec3.h"
#include "Quaternion.h"

struct Transform
{
    Quaternion rotation;
    Vec3 position;
    Vec3 scale;
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
    }
};