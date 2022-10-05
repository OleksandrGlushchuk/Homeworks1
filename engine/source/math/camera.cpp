#include "camera.h"
#include "MatrixHelpers.h"
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

	float w;
	TopLeft.mult(m_viewProjInv, 1, &w);
	TopLeft /= w;
	TopLeft -= position();

	BottomLeft.mult(m_viewProjInv, 1, &w);
	BottomLeft /= w;
	BottomLeft -= position();

	BottomRight.mult(m_viewProjInv, 1, &w);
	BottomRight /= w;
	BottomRight -= position();

	BR_M_BL = BottomRight - BottomLeft;
	TL_M_BL = TopLeft - BottomLeft;
}

void Camera::updateAspect(float _aspect)
{
	aspect = _aspect;
	setPerspective(fov, aspect, p_near, p_far);
	need_to_update_basis = true;
	need_to_update_matrices = true;
	updateBasis();
	updateMatrices();
}

void Camera::updateMatrices()
{
	if (need_to_update_matrices)
	{
		updateBasis();
		m_view = m_viewInv.invert();
		m_viewProj = m_view * m_proj;
		m_viewProjInv = m_projInv * m_viewInv;
		need_to_update_matrices = false;
		updateCorners();
	}
}

void Camera::updateBasis()
{
	if (need_to_update_basis)
	{
		Matr<3> quat_to_matr = m_rotation.toMat3();
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
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
	updateMatrices();
}

void Camera::addWorldOffset(const Vec3& offset)
{
	need_to_update_matrices = false;
	Matr<4>::add_to_row(m_viewInv[3] , offset);
}

void Camera::addRelativeOffset(const Vec3 &offset)
{
	Matr<4>::add_to_row(m_viewInv[3] , offset[0] * right() + offset[1] * top() + offset[2] * forward());
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
	math::setPerspective(fov, aspect, p_near, p_far, m_proj);
	m_projInv = m_proj.invert();
}
