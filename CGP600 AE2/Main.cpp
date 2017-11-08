#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include "GameSystem.h"

//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	GameSystem* gameSystem = new GameSystem;
	MSG msg = { 0 };

	gameSystem->playGame(msg, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	if (gameSystem)
	{
		delete gameSystem;
		gameSystem = nullptr;
	}

	return (int)msg.wParam;
}
