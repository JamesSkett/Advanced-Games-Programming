#include "MainMenu.h"



MainMenu::MainMenu()
{

}

//Destroy objects when done
MainMenu::~MainMenu()
{
	if (m_playGame)
	{
		delete m_playGame;
		m_playGame = nullptr;
	}

	if (m_exitGame)
	{
		delete m_exitGame;
		m_exitGame = nullptr;
	}
}

//Starts the main menu
void MainMenu::StartMenu(ID3D11Device * D3D11Device, ID3D11DeviceContext * ImmediateContext, Renderer* renderer)
{
	//Create the menu buttons
	m_playGame = new Text2D("assets/MyFont.png", D3D11Device, ImmediateContext);
	m_exitGame = new Text2D("assets/MyFont.png", D3D11Device, ImmediateContext);

	bool isDone = false;

	//Menu Loop
	while (!isDone)
	{

		if (renderer->IsKeyPressed(DIK_W))
		{
			//if already on play game keep scale the same;
			if (m_onPlay)
			{
				m_playTextScale = 0.1f;
			}
			else if (m_onExit) //else set the exit button scale larger and swap the boolean values
			{
				m_playTextScale = 0.1f;
				m_exitTextScale = 0.05f;
				m_onExit = false;
				m_onPlay = true;
			}
		}

		if (renderer->IsKeyPressed(DIK_S))
		{
			//if already on exit game keep scale the same;
			if (m_onExit)
			{
				m_exitTextScale = 0.1f;
			}
			else if (m_onPlay) //else set the play button scale larger and swap the boolean values
			{
				m_playTextScale = 0.05f;
				m_exitTextScale = 0.1f;
				m_onPlay = false;
				m_onExit = true;
			}
		}

		//When enter is pressed
		if (renderer->IsKeyPressed(DIK_RETURN))
		{
			if (m_onPlay) // Menu loop is done play the game
			{
				isDone = true;
			}

			if (m_onExit) // exit the menu loop and destroy the window to exit game
			{
				renderer->DestroyWin();
				isDone = true;
			}
		}

		// Add the text for the buttons
		m_playGame->AddText("Play Game", -0.3f, 0.3f, m_playTextScale);
		m_exitGame->AddText("Exit Game", -0.3f, -0.3f, m_exitTextScale);

		//render the graphics
		renderer->RenderFrame(m_playGame, m_exitGame, isDone);
	}
}
