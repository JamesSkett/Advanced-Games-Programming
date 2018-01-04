#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>

#include "Renderer.h"
#include "Scene_Node.h"
#include "Mesh.h"
#include "Math.h"
#include "text2D.h"
#include "Projectile.h"

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

class GameSystem
{
public:
	GameSystem();
	~GameSystem();

	int playGame(MSG msg, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void SetupLevel();

	void GetKeyboardInput();
	void GetControllerInput();

	CXBOXController* player1 = new CXBOXController(1);
	static Math* math;

private:
	Renderer* renderer;

	Mesh* m_spaceShip;
	Mesh* m_planet;
	Mesh* m_shipGuns;

	Scene_Node* m_root_node;

	Scene_Node* m_spaceship_node;
	Scene_Node* m_planet_node;
	Scene_Node* m_shipGun1_node;
	Scene_Node* m_shipGun2_node;

	Mesh* m_bulletMesh;
	vector <Projectile*> m_shipBullets;

	//Text2D* text;

	int m_bulletNum = 0;
	bool m_isMousePressed = false;

	bool m_leftGun = true;
	bool m_rightGun = false;
};

