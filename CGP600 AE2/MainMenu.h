#pragma once
#include "text2D.h"
#include "Renderer.h"

class MainMenu
{
public:
	MainMenu();

	//Destroy objects when done
	~MainMenu();

	//Starts the main menu
	void StartMenu(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext, Renderer* renderer);

private:
	Text2D* m_playGame;
	float m_playTextScale = 0.1f;
	bool m_onPlay = true;

	Text2D* m_exitGame;
	float m_exitTextScale = 0.05f;
	bool m_onExit = false;

	bool m_isKeyPressed = false;
};

