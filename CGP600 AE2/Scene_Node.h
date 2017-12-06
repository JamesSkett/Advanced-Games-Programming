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

	bool UpdateXPos(float distance, Scene_Node* rootNode);
	bool UpdateYPos(float distance, Scene_Node* rootNode);
	bool UpdateZPos(float distance, Scene_Node* rootNode);
	bool UpdateXangle(float angle, Scene_Node* rootNode);
	bool UpdateYangle(float angle, Scene_Node* rootNode);
	bool UpdateZangle(float angle, Scene_Node* rootNode);

	void SetModel(Mesh* mesh);
	void AddChildNode(Scene_Node* n);

	bool DetachNode(Scene_Node* n);

	void Execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);

	XMVECTOR GetWorldCentrePos();
	void UpdateCollisionTree(XMMATRIX* world, float scale);

	bool CheckCollision(Scene_Node* compareTree);
	bool CheckCollision(Scene_Node * compareTree, Scene_Node * objectTreeRoot);

	void setCanCollide(bool canCollide);
	void setCanDraw(bool canDraw);

private:
	Mesh* m_pModel;
	vector <Scene_Node*> m_children;

	float m_x, m_y, m_z;
	float m_xangle, m_yangle, m_zangle;
	float m_scale;

	float m_world_centre_x;
	float m_world_centre_y;
	float m_world_centre_z;

	float m_world_scale;

	bool m_canCollide = true;
	bool m_canDraw = true;
};

