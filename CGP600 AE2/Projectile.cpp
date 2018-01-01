#include "Projectile.h"



Projectile::Projectile(float velocity)
{
	m_velocity = velocity;
}


Projectile::~Projectile()
{
}

void Projectile::UpdateProjectile(Scene_Node* root_node)
{
	if (m_isFired)
	{
		xyz startPos = { m_x, m_y, m_z };
		xyz currentPos;
		float distance;
		bool inRange = true;
		while(inRange)
		{
			currentPos = { m_x, m_y, m_z };
			MoveForward(m_velocity, root_node);

			distance = sqrt(pow(currentPos.x - startPos.x, 2) + pow(currentPos.y - startPos.y, 2) + pow(currentPos.z - startPos.z, 2));

			if (distance < 100.0f)
			{

			}
		}
	}
}
