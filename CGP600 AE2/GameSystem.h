#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include "Renderer.h"
#include "Scene_Node.h"
#include "Mesh.h"
#include "Math.h"
#include "text2D.h"

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
	Mesh* mesh2;
	Mesh* m_shipGun1;

	Scene_Node* m_root_node;

	Scene_Node* m_spaceship_node;
	Scene_Node* m_node2;
	Scene_Node* m_shipGun1_node;

	Text2D* text;
};

