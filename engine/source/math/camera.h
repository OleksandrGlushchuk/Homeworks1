#pragma once
#include "vec3.h"
#include "Matr.h"
#include "Quaternion.h"

class Camera
{
	bool need_to_update_matrices = true;
	bool need_to_update_basis = true;
	bool need_to_roll = false;
public:
	const Vec3& right() 	const { return m_viewInv[0]; }
	const Vec3& top() 		const { return m_viewInv[1]; }
	const Vec3& forward() 	const { return m_viewInv[2]; }
	const Vec3& position()	const { return m_viewInv[3]; }

	Matr<4> m_proj = Matr<4>::identity(),		m_projInv = Matr<4>::identity(),
			m_view = Matr<4>::identity(),		m_viewInv = Matr<4>::identity(),
			m_viewProj = Matr<4>::identity(),	m_viewProjInv = Matr<4>::identity();
	Quaternion m_rotation;
	Vec3 TopLeft, BottomLeft, BottomRight;
	Vec3 BR_M_BL, TL_M_BL;
	float fov, aspect, p_near, p_far;
	Camera() {};
	Camera(float _fov, float _aspect, float _p_near, float _p_far);

	void updateCorners();
	void updateAspect(float _aspect);
	void updateMatrices();
	void addRelativeOffset(const Vec3 &offset);
	void setPerspective(float fov, float aspect, float p_near, float p_far); // sets m_proj and m_projInv
	void addRelativeAngles(const Angles& angles);
	void updateBasis();

	void setWorldOffset(const Vec3& offset);
	void addWorldAngles(const Angles& angles);
	void addWorldOffset(const Vec3& offset);
	void setWorldAngles(const Angles& angles);

	friend class Application;
};