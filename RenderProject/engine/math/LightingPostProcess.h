#pragma once
#include "vec3.h"
#include <algorithm>
namespace lpp
{
	inline Vec3 AdjustExposure(const Vec3& radiance, float EV100)
	{
		float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
		return radiance * (1.0f / LMax);
	}

	inline Vec3 ACES(const Vec3& hdr)
	{
		Matr<3> m1;
		Matr<3>::fill_row(m1[0], { 0.59719f, 0.07600f, 0.02840f });
		Matr<3>::fill_row(m1[1], { 0.35458f, 0.90834f, 0.13383f });
		Matr<3>::fill_row(m1[2], { 0.04823f, 0.01566f, 0.83777f });

		Matr<3> m2;
		Matr<3>::fill_row(m2[0], { 1.60475f, -0.10208, -0.00327f });
		Matr<3>::fill_row(m2[1], { -0.53108f, 1.10813, -0.07276f });
		Matr<3>::fill_row(m2[2], { -0.07367f, -0.00605, 1.07602f });

		Vec3 v = hdr;
		v.mult(m1);
		Vec3 a = v * (v + Vec3(0.0245786f, 0.0245786f, 0.0245786f)) - Vec3(0.000090537f, 0.000090537f, 0.000090537f);
		Vec3 b = v * (0.983729f * v + Vec3(0.4329510f, 0.4329510f, 0.4329510f)) + Vec3(0.238081f, 0.238081f, 0.238081f);

		Vec3 ldr = (a / b).mult(m2);
		ldr.e[0] = std::clamp(ldr.e[0], 0.0f, 1.0f);
		ldr.e[1] = std::clamp(ldr.e[1], 0.0f, 1.0f);
		ldr.e[2] = std::clamp(ldr.e[2], 0.0f, 1.0f);
		return ldr;
	}

	inline Vec3 GammaCorrection(const Vec3& radiance)
	{
		return Vec3(
			powf(radiance.e[0], 1.f / 2.2f),
			powf(radiance.e[1], 1.f / 2.2f),
			powf(radiance.e[2], 1.f / 2.2f)
		);
	}
}