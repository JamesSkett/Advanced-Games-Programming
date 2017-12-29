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
	void Rotate(float number_of_degrees, float objectX, float objectY, float objectZ);
	void Forward(float distance, Scene_Node* rootNode);
	void Up(float distance);
	void Strafe(float distance);
	void Pitch(float number_of_degrees);

	//setters
	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetYaw(float angle);
	void SetPitch(float angle);
	void SetIsForward(bool isForward);
	void SetIsBackward(bool isBackward);
	void SetIsRight(bool isRight);
	void SetIsLeft(bool isLeft);

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
	float m_camPitch = 0.0f;
	float m_camYaw = 0.0f;
	float m_moveLeftRight = 0.0f;
	float m_moveBackForward = 0.0f;

	XMVECTOR m_position = XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f);
	XMVECTOR m_lookat;
	XMVECTOR m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR m_defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR m_defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR m_forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR m_right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMMATRIX m_camRotationMatrix;
	XMMATRIX m_groundWorld;

	//Used for changing direction camera is facing
	bool m_isForward, m_isBackward, m_isRight, m_isLeft;
};

