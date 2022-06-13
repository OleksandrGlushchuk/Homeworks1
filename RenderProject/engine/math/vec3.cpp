#include "vec3.h"
#include "Matr.h"

Vec3& Vec3::mult(const Matr<4>& matr, float w)
{
    Vec3 vec;
    float sum = 0;
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
        {
            sum += e[j] * matr[j][i];
        }
        sum += w * matr[3][i];
        vec[i] = sum;
    }
    for (int i = 0; i < 3; i++)
    {
        e[i] = vec[i];
    }
    return *this;
}

Vec3& Vec3::mult(const Matr<3>& matr)
{
    Vec3 vec;
    float sum = 0;
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
        {
            sum += e[j] * matr[j][i];
        }
        vec[i] = sum;
    }
    for (int i = 0; i < 3; i++)
    {
        e[i] = vec[i];
    }
    return *this;
}