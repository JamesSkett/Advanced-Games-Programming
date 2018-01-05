#include "Enemy.h"


Enemy::Enemy(int health, float x, float y, float z, float size)
{
	m_health = health;

	m_x = x; 
	m_y = y; 
	m_z = z;

	m_scale = size;
}

Enemy::~Enemy()
{
}

void Enemy::UpdateEnemy(Scene_Node* player)
{
	float distance = sqrt(pow(m_x - player->GetXPos(), 2) + pow(m_y - player->GetYPos(), 2) + pow(m_z - player->GetZPos(), 2));

	if (distance < 100)
	{
		MoveTowardsPlayer(player->GetXPos(), player->GetYPos(), player->GetZPos());
	}

	if (m_health <= 0) //planet is destryed dont render or check collision
	{
		m_canDraw = false;
		m_canCollide = false;
	}
}

void Enemy::MoveTowardsPlayer(float targetx, float targety, float targetz)
{
	m_direction.x = targetx - m_x;
	m_direction.y = targety - m_y;
	m_direction.z = targetz - m_z;

	float magnitude = sqrt(m_direction.x*m_direction.x + m_direction.y*m_direction.y + m_direction.z);

	m_direction.x /= magnitude;
	m_direction.y /= magnitude;
	m_direction.z /= magnitude;

	m_x += m_direction.x;
	m_y += m_direction.y;
	m_z += m_direction.z;
}

void Enemy::RemoveHealth(int health)
{
	m_health -= health;
}