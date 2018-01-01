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

	void MoveForward(float speed);

private:
	bool m_isFired = false;
	float m_velocity;
};

