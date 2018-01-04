#pragma once
#include "Scene_Node.h"

class Projectile :
	public Scene_Node
{
public:
	Projectile(float velocity);
	~Projectile();

	bool UpdateProjectile(Scene_Node* root_node);

	void SetIsFired(bool isFired);
	void SetStartPos(float x, float y, float z);

	void setDirection(float dx, float dy, float dz);

	bool GetIsFired();


	void MoveForward(float speed);

private:
	bool m_isFired = false;
	float m_velocity;

	float m_localDirX;
	float m_localDirY;
	float m_localDirZ;

	xyz m_startPos;
};

