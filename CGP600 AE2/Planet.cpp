#include "Planet.h"
#include "Renderer.h"


// Set up the planet properties
Planet::Planet(int health, float size, float xPos, float yPos, float zPos)
{
	m_health = health;
	m_scale = size;
	m_x = xPos;
	m_y = yPos;
	m_z = zPos;

	healthText = new Text2D("assets/MyFont.png", Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);



}

//delete the object when done
Planet::~Planet()
{
	if (healthText)
	{
		delete healthText;
		healthText = nullptr;
	}
}

//updates the planet every frame
void Planet::UpdatePlanet(Scene_Node* playerShip)
{
	UpdateYangle(0.5f);

	string m_healthString = "Planet Health:";

	m_healthString = m_healthString + to_string(m_health);


	float distance = sqrt(pow(m_x - playerShip->GetXPos(), 2) + pow(m_y - playerShip->GetYPos(), 2) + pow(m_z - playerShip->GetZPos(), 2));
	
	if (distance < 150) //display the health when within range
	{
		healthText->AddText(m_healthString, -0.5, -0.8, 0.04);
	}

	if (m_health <= 0) //planet is destryed dont render or check collision
	{
		m_canDraw = false;
		m_canCollide = false;
	}

}

//Renders the text when called
void Planet::RenderText()
{
	healthText->RenderText();
}

//rotates the planet round the y axis
void Planet::UpdateYangle(float angle)
{
	m_yangle += angle;
}

//removes health from the planet
void Planet::RemoveHealth(int value)
{
	m_health -= value;
}
