#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>

#include "Renderer.h"
#include "MainMenu.h"
#include "Scene_Node.h"
#include "Mesh.h"
#include "Math.h"
#include "text2D.h"
#include "Projectile.h"
#include "Planet.h"



class GameSystem
{
public:
	GameSystem();
	~GameSystem();

	//play the main game loop
	int playGame(MSG msg, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	
	//set up all the objects in the game
	void SetupLevel();

	//get device input
	void GetKeyboardInput();
	void GetControllerInput();
	CXBOXController* player1 = new CXBOXController(1);

	//can use this for equations
	static Math math;

private:
	Renderer* renderer;

	MainMenu* m_mainMenu;

	//object meshes
	Mesh* m_spaceShip;
	Mesh* m_planet;
	Mesh* m_shipGuns;

	//Scene management nodes
	Scene_Node* m_root_node;
	Scene_Node* m_spaceship_node;
	Scene_Node* m_shipGun1_node;
	Scene_Node* m_shipGun2_node;

	//Ship bullet mesh and projectile vector
	Mesh* m_bulletMesh;
	vector <Projectile*> m_shipBullets;

	//vector of planets
	vector <Planet*> m_planets;

	//bullet shooting values
	int m_bulletNum = 0;
	bool m_isMousePressed = false;
	bool m_leftGun = true;
	bool m_rightGun = false;
};

enum ShipGuns
{
	NUMBER_OF_GUNS = 2,
	FRONT_LEFT_GUN = 0,
	FRONT_RIGHT_GUN = 1
};

enum ShipBullets
{
	NUM_OF_BULLETS = 50,
};

enum planet0
{
	PLANET_0_X_POS = 100,
	PLANET_0_Y_POS = 10,
	PLANET_0_Z_POS = 300,
	PLANET_0_SIZE = 20,
	PLANET_0_HEALTH = 100
};

enum planet1
{
	PLANET_1_X_POS = -200,
	PLANET_1_Y_POS = -110,
	PLANET_1_Z_POS = 200,
	PLANET_1_SIZE = 10,
	PLANET_1_HEALTH = 75
};