#pragma once
#include "vec3.h"
#include "Matr.h"

namespace math
{
	inline void setView(const Vec3& view_pos, const Vec3& view_dir, Matr<4>& out_view_matr)
	{
		Vec3 Y(0, 1, 0);
		Vec3 X = Vec3::cross(Y, view_dir).normalize();
		Y = Vec3::cross(view_dir, X);
		Matr<4>::fill_row(out_view_matr[0], { X.e[0],					Y.e[0],					view_dir.e[0],					0 });
		Matr<4>::fill_row(out_view_matr[1], { X.e[1],					Y.e[1],					view_dir.e[1],					0 });
		Matr<4>::fill_row(out_view_matr[2], { X.e[2],					Y.e[2],					view_dir.e[2],					0 });
		Matr<4>::fill_row(out_view_matr[3], { -Vec3::dot(X,view_pos),	-Vec3::dot(Y,view_pos), -Vec3::dot(view_dir,view_pos),	1 });
	}

	inline void setPerspective(float fov, float aspect, float p_near, float p_far, Matr<4>& out_proj_matr)
	{
		Matr<4>::fill_row(out_proj_matr[0], { (1.f / tanf(fov / 2.f)) / aspect,		0,							0,									0 });
		Matr<4>::fill_row(out_proj_matr[1], { 0,									1.f / tanf(fov / 2.f),		0,									0 });
		Matr<4>::fill_row(out_proj_matr[2], { 0,									0,							p_near / (p_near - p_far),			1 });
		Matr<4>::fill_row(out_proj_matr[3], { 0,									0,							-p_far * p_near / (p_near - p_far),	0 });
	}

	inline void setOrtho(float left, float right, float bottom, float top, float near, float far, Matr<4>& out_matr)
	{
		Matr<4>::fill_row(out_matr[0], { 2.f / (right - left),				0,									0,								0 });
		Matr<4>::fill_row(out_matr[1], { 0,									2.f / (top - bottom),				0,								0 });
		Matr<4>::fill_row(out_matr[2], { 0,									0,									1.f / (near - far),				0 });
		Matr<4>::fill_row(out_matr[3], { (right + left) / (left - right),	(top + bottom) / (bottom - top),	(far) / (far - near),			1 });
	}
}