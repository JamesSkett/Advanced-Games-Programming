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

	m_dx = sinf(XMConvertToRadians(m_camera_rotation));
	m_dz = cosf(XMConvertToRadians(m_camera_rotation));
	m_dy = sinf(XMConvertToRadians(m_camera_rotation));

	m_isForward = true;
	m_isBackward = false;
	m_isRight = false;
	m_isLeft = false;
}

Camera::~Camera()
{
}

void Camera::Rotate(float number_of_degrees, float objectX, float objectY, float objectZ)
{
	/*m_camera_rotation += number_of_degrees;

	m_dx = sinf(XMConvertToRadians(m_camera_rotation));
	m_dy = sinf(XMConvertToRadians(m_camera_rotation));
	m_dz = cosf(XMConvertToRadians(m_camera_rotation));*/

	float s = sinf(XMConvertToRadians(number_of_degrees));
	float c = cosf(XMConvertToRadians(number_of_degrees));

	m_x -= objectX;
	m_y -= objectZ;

	float xnew = (m_x * c) - (m_y * s);
	float znew = (m_y * s) + (m_x * c);

	m_x = znew;
	m_y = xnew;
}

void Camera::Forward(float distance, Scene_Node* rootNode)
{
	float oldX = m_x;
	float oldZ = m_z;

	m_x += m_dx * distance;
	m_z += m_dz * distance;

	xyz camPos = { m_x, m_y, m_z };
	xyz camForward = { XMVectorGetX(m_forward) / 2, XMVectorGetY(m_forward) / 2, XMVectorGetZ(m_forward) / 2 };

	if (rootNode->CheckCollisionRay(&camPos, &camForward, true))
	{
		m_x = oldX;
		m_z = oldZ;
	}
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

	m_dx = sinf(XMConvertToRadians(m_camera_rotation));
	m_dy = sinf(XMConvertToRadians(m_camera_rotation));
	m_dz = cosf(XMConvertToRadians(m_camera_rotation));
	
	m_up = XMVector3Cross(m_right, m_lookat);

}

void Camera::SetX(float x)
{
	m_x = x;
}

void Camera::SetY(float y)
{
	m_y = y;
}

void Camera::SetZ(float z)
{
	m_z = z;
}

void Camera::SetYaw(float angle)
{
	m_camYaw = angle;
}

void Camera::SetPitch(float angle)
{
	m_camPitch = angle;
}

void Camera::SetIsForward(bool isForward)
{
	m_isForward = isForward;
}

void Camera::SetIsBackward(bool isBackward)
{
	m_isBackward = isBackward;
}

void Camera::SetIsRight(bool isRight)
{
	m_isRight = isRight;
}

void Camera::SetIsLeft(bool isLeft)
{
	m_isLeft = isLeft;
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

float Camera::GetDirectionX()
{
	return m_dx;
}

float Camera::GetDirectionY()
{
	return m_dy;
}

float Camera::GetDirectionZ()
{
	return m_dz;
}

// makes the camera follow the position of an object with a slight offset
void Camera::CameraFollow(float targetX, float targetY, float targetZ)
{
	if (m_isForward) //if isForward is true the camera faces down the z axis positively
	{
		m_x = targetX;
		m_y = targetY + 3;
		m_z = targetZ - 15;

		m_dx = sinf(XMConvertToRadians(0.0f));
		m_dz = cosf(XMConvertToRadians(0.0f));
	}
	else if(m_isBackward) //if isBackward is true the camera faces down the z axis negatively
	{
		m_x = targetX;
		m_y = targetY + 3;
		m_z = targetZ + 15;

		m_dx = sinf(XMConvertToRadians(180.0f));
		m_dz = cosf(XMConvertToRadians(180.0f));
	}
	else if (m_isRight) //if isRight is true the camera faces down the x axis negatively
	{
		m_x = targetX + 15;
		m_y = targetY + 3;
		m_z = targetZ;

		m_dx = sinf(XMConvertToRadians(270.0f));
		m_dz = cosf(XMConvertToRadians(270.0f));
	}
	else if (m_isLeft) //if isLeft is true the camera faces down the y axy positively
	{
		m_x = targetX - 15;
		m_y = targetY + 3;
		m_z = targetZ;

		m_dx = sinf(XMConvertToRadians(90.0f));
		m_dz = cosf(XMConvertToRadians(90.0f));
	}

	
}

void Camera::LookAt(float targetX, float targetZ)
{
	m_camera_rotation = atan2f((targetX - m_x), (targetZ - m_z)) * (180 / XM_PI);

	m_dx = sinf(XMConvertToRadians(m_camera_rotation));
	m_dz = cosf(XMConvertToRadians(m_camera_rotation));
}


XMMATRIX Camera::GetViewMatrix()
{
	/*m_camRotationMatrix = XMMatrixRotationRollPitchYaw(m_camPitch, m_camYaw, 0);
	m_lookat = XMVector3TransformCoord(m_defaultForward, m_camRotationMatrix);
	m_lookat = XMVector3Normalize(m_lookat);*/

	//XMMATRIX rotateYTempMatrix;
	//rotateYTempMatrix = XMMatrixRotationY(m_camYaw);

	//m_right = XMVector3TransformCoord(m_defaultRight, rotateYTempMatrix);
	//m_up = XMVector3TransformCoord(m_up, rotateYTempMatrix);
	//m_forward = XMVector3TransformCoord(m_defaultForward, rotateYTempMatrix);

	/*m_position += m_moveLeftRight * m_right;
	m_position += m_moveBackForward * m_forward;

	m_moveLeftRight = 0.0f;
	m_moveBackForward = 0.0f;

	m_lookat = m_position + m_lookat;*/


	m_position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	m_lookat = XMVectorSet(m_x + m_dx, m_y + m_dy, m_z + m_dz, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	//Get the forward and right vector positions
	m_forward = XMVector3Normalize(m_lookat - m_position);
	m_right = XMVector3Cross(m_forward, m_up);

	return XMMatrixLookAtLH(m_position, m_lookat, m_up);
}

