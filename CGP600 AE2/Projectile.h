#pragma once
#include "Scene_Node.h"

class Projectile :
	public Scene_Node
{
public:
	Projectile(float velocity);
	~Projectile();

	void UpdateProjectile(Scene_Node* root_node);

private:
	bool m_isFired = false;
	float m_velocity;
};

