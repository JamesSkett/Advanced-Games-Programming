#pragma once

//#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
//#define _XM_NO_INTRINSICS
//#define XM_NO_ALIGNMENT
//#include <xnamath.h>

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera(float x, float y, float z, float rotation);
	~Camera();

	void Rotate(float number_of_degrees);
	void Forward(float distance);
	void Up(float distance);
	void Strafe(float distance);
	void Pitch(float number_of_degrees);

	XMMATRIX GetViewMatrix();

private:
	float m_x, m_y, m_z;
	float m_dx, m_dy, m_dz;
	float m_camera_rotation;

	XMVECTOR m_position, m_lookat, m_up, m_right, m_forward;
};

