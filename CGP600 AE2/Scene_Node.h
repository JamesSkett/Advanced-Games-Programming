#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h>
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

#include "Mesh.h"

class Scene_Node
{
public:
	Scene_Node();
	~Scene_Node();

	//Getters
	float GetXPos();
	float GetYPos();
	float GetZPos();
	float GetXAngle();
	float GetYAngle();
	float GetZAngle();
	float GetScale();

	//Setters
	void SetXPos(float x);
	void SetYPos(float y);
	void SetZPos(float z);
	void SetXAngle(float xAngle);
	void SetYAngle(float yAngle);
	void SetZAngle(float zAngle);
	void SetScale(float scale);

	void UpdateXPos(float distance);
	void UpdateYPos(float distance);
	void UpdateZPos(float distance);
	void UpdateXangle(float angle);
	void UpdateYangle(float angle);
	void UpdateZangle(float angle);

	void SetModel(Mesh* mesh);
	void AddChildNode(Scene_Node* n);

	bool DetachNode(Scene_Node* n);

	void Execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);

private:
	Mesh* m_pModel;
	vector <Scene_Node*> m_children;

	float m_x, m_y, m_z;
	float m_xangle, m_yangle, m_zangle;
	float m_scale;
};

