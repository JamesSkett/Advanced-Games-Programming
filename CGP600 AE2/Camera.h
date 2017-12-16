#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include "Scene_Node.h"

__declspec(align(16)) class Camera
{
public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		return _mm_free(p);
	}

	Camera(float x, float y, float z, float rotation);
	~Camera();

	//Functions to manipulate the camera
	void Rotate(float number_of_degrees);
	void Forward(float distance, Scene_Node* rootNode);
	void Up(float distance);
	void Strafe(float distance);
	void Pitch(float number_of_degrees);

	//get the x,y,z positions of the camera
	float GetX();
	float GetY();
	float GetZ();
	float GetDirectionX();
	float GetDirectionY();
	float GetDirectionZ();

	void CameraFollow(float targetX, float targetY, float targetZ);

	void LookAt(float targetX, float targetZ);

	XMMATRIX GetViewMatrix();

private:
	float m_x, m_y, m_z;
	float m_dx, m_dy, m_dz;
	float m_camera_rotation;

	XMVECTOR m_position, m_lookat, m_up, m_right, m_forward;
};

