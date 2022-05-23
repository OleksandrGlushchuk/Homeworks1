#pragma once
#include "vec3.h"
#include "Matr.h"
#include "Quaternion.h"

class Angles
{
public:
	float roll, pitch, yaw;
	Angles(float _roll=0, float _pitch=0, float _yaw=0):roll(_roll),pitch(_pitch),yaw(_yaw){}
};

class Camera
{
	bool need_to_update_matrices = true;
	bool need_to_update_basis = true;
public:
	const Vec3& right() 	const { return m_viewInv[0]; }
	const Vec3& top() 		const { return m_viewInv[1]; }
	const Vec3& forward() 	const { return m_viewInv[2]; }
	const Vec3& position()	const { return m_viewInv[3]; }

	Matr m_proj = Matr(4), m_projInv = Matr(4);
	Matr m_view = Matr(4), m_viewInv = Matr(4);
	Matr m_viewProj = Matr(4), m_viewProjInv = Matr(4);
	Quaternion m_rotation;
	Camera();
	void updateMatrices();
	void addRelativeOffset(const Vec3 &offset);
	void setPerspective(float fov, float aspect, float p_near, float p_far); // sets m_proj and m_projInv
	void addRelativeAngles(const Angles& angles);
	void updateBasis();

	void setWorldOffset(const Vec3& offset);
	void addWorldAngles(const Angles& angles);
	void addWorldOffset(const Vec3& offset);
	void setWorldAngles(const Angles& angles);

	friend class Controller;
};