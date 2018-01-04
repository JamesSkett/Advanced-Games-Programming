#pragma once
#include "Scene_Node.h"
#include "text2D.h"

class Planet :
	public Scene_Node
{
public:
	// Set up the planet properties
	Planet(int health, float size, float xPos, float yPos, float zPos);
	//delete the object when done
	~Planet();

	//updates the planet every frame
	void UpdatePlanet(Scene_Node* playerShip);

	//Renders the text when called
	void RenderText();

	//rotates the planet round the y axis
	void UpdateYangle(float angle);

	//removes health from the planet
	void RemoveHealth(int value);

private:
	int m_health;

	Text2D* healthText;
};

