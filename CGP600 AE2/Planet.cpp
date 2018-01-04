#include "Planet.h"
#include "Renderer.h"

Planet::Planet(int health, float size, float xPos, float yPos, float zPos, char* textureFile)
{
	m_health = health;
	m_scale = size;
	m_x = xPos;
	m_y = yPos;
	m_z = zPos;

	healthText = new Text2D("assets/MyFont.png", Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);



}


Planet::~Planet()
{
	if (healthText)
	{
		delete healthText;
		healthText = nullptr;
	}
}

void Planet::UpdatePlanet(Scene_Node* playerShip)
{
	UpdateYangle(0.5f);

	string m_healthString = "Planet Health:";

	m_healthString = m_healthString + to_string(m_health);


	float distance = sqrt(pow(m_x - playerShip->GetXPos(), 2) + pow(m_y - playerShip->GetYPos(), 2) + pow(m_z - playerShip->GetZPos(), 2));
	if (distance < 200)
	{
		healthText->AddText(m_healthString, -0.5, -0.8, 0.04);
	}

}

void Planet::RenderText()
{
	healthText->RenderText();
}

void Planet::UpdateYangle(float angle)
{
	m_yangle += angle;
}

void Planet::RemoveHealth(int value)
{
	m_health -= value;
}
