#include "camera.h"

std::vector<float>& operator+=(std::vector<float>& vf, const Vec3& v3)
{
	vf[0] += v3[0];
	vf[1] += v3[1];
	vf[2] += v3[2];
	return vf;
}

Camera::Camera()
{
	m_view[0] = { 1,0,0,0 };
	m_view[1] = { 0,1,0,0 };
	m_view[2] = { 0,0,1,0 };
	m_view[3] = { 0,0,0,1 };
	m_viewInv = invert(m_view);

}

Camera::Camera(float _fov, float _aspect, float _p_near, float _p_far) : fov(_fov), aspect(_aspect), p_near(_p_near), p_far(_p_far)
{
	setPerspective(fov, aspect, p_near, p_far);
	setWorldOffset(Vec3(0, 0, 0));
	setWorldAngles(Angles(0, 0, 0));
	updateBasis();
	updateMatrices();
}

void Camera::updateCorners()
{
	TopLeft = Vec3(-1, 1, 1);
	BottomLeft = Vec3(-1, -1, 1); BottomRight = Vec3(1, -1, 1);

	TopLeft.mult(m_viewProjInv, 1) / m_viewProjInv[3][3] - position();
	BottomLeft.mult(m_viewProjInv, 1) / m_viewProjInv[3][3] - position();
	BottomRight.mult(m_viewProjInv, 1) / m_viewProjInv[3][3] - position();

	BR_M_BL = BottomRight - BottomLeft;
	TL_M_BL = TopLeft - BottomLeft;
}

void Camera::updateAspect(float _aspect)
{
	aspect = _aspect;
	setPerspective(fov, aspect, p_near, p_far);
	need_to_update_basis = true;
	need_to_update_matrices = true;
}

void Camera::updateMatrices()
{
	if (need_to_update_matrices)
	{
		updateBasis();
		m_view = invert(m_viewInv);
		m_viewProj = m_view * m_proj;
		m_viewProjInv = m_projInv * m_viewInv;
		need_to_update_matrices = false;
	}
}

void Camera::updateBasis()
{
	if (need_to_update_basis)
	{
		Matr quat_to_matr = m_rotation.toMat3();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m_viewInv[i][j] = quat_to_matr[i][j];
			}
		}
		need_to_update_basis = false;
	}
}

void Camera::setWorldOffset(const Vec3& offset)
{
	need_to_update_matrices = true;
	m_viewInv[3][0] = offset.e[0];
	m_viewInv[3][1] = offset.e[1];
	m_viewInv[3][2] = offset.e[2];
}

void Camera::addWorldOffset(const Vec3& offset)
{
	need_to_update_matrices = false;
	m_viewInv[3] += offset;
}

void Camera::addRelativeOffset(const Vec3 &offset)
{
	updateBasis();
	m_viewInv[3] += offset[0] * right() + offset[1] * top() + offset[2] * forward();
	need_to_update_matrices = true;
}

void Camera::addWorldAngles(const Angles& angles)
{
	m_rotation *= Quaternion(angles.roll,  { 0.f, 0.f, 1.f });
	m_rotation *= Quaternion(angles.pitch, { 1.f, 0.f, 0.f });
	m_rotation *= Quaternion(angles.yaw,   { 0.f, 1.f, 0.f });

	m_rotation.normalize();

	need_to_update_basis = true;
	need_to_update_matrices = true;
}

void Camera::setWorldAngles(const Angles& angles)
{
	m_rotation = Quaternion(angles.roll, { 0.f, 0.f, 1.f });
	m_rotation *= Quaternion(angles.pitch, { 1.f, 0.f, 0.f });
	m_rotation *= Quaternion(angles.yaw, { 0.f, 1.f, 0.f });
	
	m_rotation.normalize();

	need_to_update_basis = true;
	need_to_update_matrices = true;
}

void Camera::addRelativeAngles(const Angles& angles)
{
	if (need_to_roll)
	{
		m_rotation *= Quaternion(angles.roll, forward());
		m_rotation *= Quaternion(angles.pitch, right());
		m_rotation *= Quaternion(angles.yaw, top());
	}
	else
	{
		m_rotation *= Quaternion(angles.pitch, right());
		m_rotation *= Quaternion(angles.yaw, { 0.f, 1.f, 0.f });
	}

	m_rotation.normalize();

	need_to_update_basis = true;
	need_to_update_matrices = true;
}

void Camera::setPerspective(float fov, float aspect, float p_near, float p_far)
{
	m_proj[0] = { (1.f / tanf(fov / 2.f)) / aspect,	0,							0,									0 };
	m_proj[1] = { 0,								1.f / tanf(fov / 2.f),		0,									0 };
	m_proj[2] = { 0,								0,							p_near / (p_near - p_far),			1 };
	m_proj[3] = { 0,								0,							-p_far * p_near / (p_near - p_far),	0 };

	m_projInv = invert(m_proj);
}
