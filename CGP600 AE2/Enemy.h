#pragma once
#include "Scene_Node.h"
class Enemy :
	public Scene_Node
{
public:
	Enemy(int health, float x, float y, float z, float size);
	~Enemy();

	void UpdateEnemy(Scene_Node* player);

	void MoveTowardsPlayer(float x, float y, float z);

	void RemoveHealth(int health);

private:
	int m_health;
	float m_speed = 0.5;

	xyz m_direction;
};

