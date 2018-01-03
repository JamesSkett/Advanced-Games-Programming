#include "Projectile.h"



Projectile::Projectile(float velocity)
{
	m_velocity = velocity;
}


Projectile::~Projectile()
{
}

bool Projectile::UpdateProjectile(Scene_Node* root_node)
{
	if (m_isFired)
	{

		m_localDirX = sinf(XMConvertToRadians(m_yangle));
		m_localDirY = atan(XMConvertToRadians(m_xangle));
		m_localDirZ = cosf(XMConvertToRadians(m_yangle));

		xyz startPos = { m_x, m_y, m_z };
		xyz currentPos;
		float distance;
		bool inRange = true;

			
		currentPos = { m_x, m_y, m_z };
			
		MoveForward(m_velocity);

		distance = sqrt(pow(currentPos.x - startPos.x, 2) + pow(currentPos.y - startPos.y, 2) + pow(currentPos.z - startPos.z, 2));

		if (distance >= 100.0f)
		{
			m_isFired = false;

			m_x = 0.0f;
			m_y = 0.0f;
			m_z = 0.0f;
			return false;
		}
		
	}
}

void Projectile::SetIsFired(bool isFired)
{
	m_isFired = isFired;
}

void Projectile::MoveForward(float speed)
{

	m_x += m_localDirX * speed;
	m_y += -m_localDirY * speed;
	m_z += m_localDirZ * speed;

}