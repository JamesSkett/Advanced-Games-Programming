#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera(float x, float y, float z, float rotation);
	~Camera();

	//Functions to manipulate the camera
	void Rotate(float number_of_degrees);
	void Forward(float distance);
	void Up(float distance);
	void Strafe(float distance);
	void Pitch(float number_of_degrees);

	//get the x,y,z positions of the camera
	float GetX();
	float GetY();
	float GetZ();

	void CameraFollow(float targetX, float targetY, float targetZ);

	void LookAt(float targetX, float targetZ);

	XMMATRIX GetViewMatrix();

private:
	float m_x, m_y, m_z;
	float m_dx, m_dy, m_dz;
	float m_camera_rotation;

	XMVECTOR m_position, m_lookat, m_up, m_right, m_forward;
};

