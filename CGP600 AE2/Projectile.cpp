#include "Projectile.h"



Projectile::Projectile(float velocity)
{
	m_velocity = velocity;
	m_canDraw = false;
}


Projectile::~Projectile()
{
}

bool Projectile::UpdateProjectile(Scene_Node* root_node)
{
	if (m_isFired)
	{
		m_canDraw = true;

		xyz currentPos;
		float distance;
		bool inRange = true;

			
		currentPos = { m_x, m_y, m_z };
			
		MoveForward(m_velocity);

		distance = sqrt(pow(currentPos.x - m_startPos.x, 2) + pow(currentPos.y - m_startPos.y, 2) + pow(currentPos.z - m_startPos.z, 2));

		if (distance >= 1000.0f)
		{
			m_isFired = false;

			m_x = 0.0f;
			m_y = 0.0f;
			m_z = 0.0f;
			return false;
		}
		
	}

	return true;
}

void Projectile::SetIsFired(bool isFired)
{
	m_isFired = isFired;
}

void Projectile::SetStartPos(float x, float y, float z)
{
	m_startPos = { x, y, z };
}

void Projectile::setDirection(float dx, float dy, float dz)
{
	m_localDirX = dx;
	m_localDirY = dy;
	m_localDirZ = dz;
}

bool Projectile::GetIsFired()
{
	return m_isFired;
}

void Projectile::MoveForward(float speed)
{

	m_x += m_localDirX * speed;
	m_y += -m_localDirY * speed;
	m_z += m_localDirZ * speed;

}