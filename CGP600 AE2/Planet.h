#pragma once
#include "Scene_Node.h"
#include "text2D.h"

class Planet :
	public Scene_Node
{
public:
	Planet(int health, float size, float xPos, float yPos, float zPos, char* textureFile);
	~Planet();

	void UpdatePlanet(Scene_Node* playerShip);

	void RenderText();

	void UpdateYangle(float angle);

	void RemoveHealth(int value);

private:
	int m_health;

	Text2D* healthText;
};

