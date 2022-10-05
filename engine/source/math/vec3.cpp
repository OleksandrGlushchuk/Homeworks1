#include "vec3.h"
#include "Matr.h"

Vec3& Vec3::mult(const Matr<4, float>& matr, float w)
{
    Vec3 vec;
    vec[0] = e[0] * matr[0][0] + e[1] * matr[1][0] + e[2] * matr[2][0] + w * matr[3][0];
    vec[1] = e[0] * matr[0][1] + e[1] * matr[1][1] + e[2] * matr[2][1] + w * matr[3][1];
    vec[2] = e[0] * matr[0][2] + e[1] * matr[1][2] + e[2] * matr[2][2] + w * matr[3][2];

    e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
    return *this;
}

Vec3& Vec3::mult(const Matr<4, float>& matr, float w, float* w_out)
{
    float vec[4];

    vec[0] = e[0] * matr[0][0] + e[1] * matr[1][0] + e[2] * matr[2][0] + w * matr[3][0];
    vec[1] = e[0] * matr[0][1] + e[1] * matr[1][1] + e[2] * matr[2][1] + w * matr[3][1];
    vec[2] = e[0] * matr[0][2] + e[1] * matr[1][2] + e[2] * matr[2][2] + w * matr[3][2];
    vec[3] = e[0] * matr[0][3] + e[1] * matr[1][3] + e[2] * matr[2][3] + w * matr[3][3];

    e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
    *w_out = vec[3];
    
    return *this;
}

Vec3& Vec3::mult(const Matr<3, float>& matr)
{
    Vec3 vec;
    vec[0] = e[0] * matr[0][0] + e[1] * matr[1][0] + e[2] * matr[2][0];
    vec[1] = e[0] * matr[0][1] + e[1] * matr[1][1] + e[2] * matr[2][1];
    vec[2] = e[0] * matr[0][2] + e[1] * matr[1][2] + e[2] * matr[2][2];

    e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
    return *this;
}