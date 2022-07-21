#pragma once
#include "../math/vec3.h"
#include "../math/Matr.h"
struct PerFrameBuffer
{
	Matr<4> viewProj;
	Vec3 BL;
	float padding0 = 0;
	Vec3 Right;
	float padding1 = 0;
	Vec3 Top;
	float padding2 = 0;
	Matr<4> viewProjInv;
	PerFrameBuffer() {}
	PerFrameBuffer(const Matr<4>& _viewProj, const Vec3& _BL, const Vec3& _Right, const Vec3& _Top, const Matr<4>& _viewProjInv) : 
		viewProj(_viewProj), BL(_BL), Right(_Right), Top(_Top), viewProjInv(_viewProjInv) {}
};
