#include "Camera.h"
#include <windows.h>
#include <stdio.h>

#include <math.h>

Camera::Camera(float x, float y, float z, float rotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_camera_rotation = rotation;

	m_dx = sin(XMConvertToRadians(m_camera_rotation));
	m_dz = cos(XMConvertToRadians(m_camera_rotation));
	m_dy = tan(XMConvertToRadians(m_camera_rotation));

}

Camera::~Camera()
{
}

void Camera::Rotate(float number_of_degrees /*float objectX, float objectY, float objectZ*/)
{
	m_camera_rotation += number_of_degrees;

	m_dx = sin(XMConvertToRadians(m_camera_rotation));
	m_dz = cos(XMConvertToRadians(m_camera_rotation));

}

void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

void Camera::Up(float distance)
{
	m_y += distance;
}

void Camera::Strafe(float distance)
{
	//move the camera left and right based on the direction
	m_position += (m_right * distance);

	//update the cameras coordinates
	m_x = XMVectorGetX(m_position);
	m_y = XMVectorGetY(m_position);
	m_z = XMVectorGetZ(m_position);


}

void Camera::Pitch(float number_of_degrees)
{
	m_camera_rotation += number_of_degrees;

	m_dy = tan(XMConvertToRadians(m_camera_rotation));
	m_dz = cos(XMConvertToRadians(m_camera_rotation));
	
	m_up = XMVector3Cross(m_right, m_lookat);

}

float Camera::GetX()
{
	return m_x;
}

float Camera::GetY()
{
	return m_y;
}

float Camera::GetZ()
{
	return m_z;
}

void Camera::CameraFollow(float targetX, float targetY, float targetZ)
{
	//m_x = targetX;
	//m_y = targetY + 3;
	m_z = targetZ - 15;
}

void Camera::LookAt(float targetX, float targetZ)
{
	m_camera_rotation = atan2((targetX - m_x), (targetZ - m_z)) * (180 / XM_PI);

	m_dx = sin(XMConvertToRadians(m_camera_rotation));
	m_dz = cos(XMConvertToRadians(m_camera_rotation));
}


XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	//Get the forward and right vector positions
	m_forward = XMVector3Normalize(m_lookat - m_position);
	m_right = XMVector3Cross(m_forward, m_up);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}
