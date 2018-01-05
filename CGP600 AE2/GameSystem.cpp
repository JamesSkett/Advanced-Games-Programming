#include "GameSystem.h"

#include <thread>

Math GameSystem::math;

//set up the renderer and main menu
GameSystem::GameSystem()
{
	renderer = new Renderer;
	m_mainMenu = new MainMenu;
}

//clean up before exiting
GameSystem::~GameSystem()
{
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
	}

	if (m_mainMenu)
	{
		delete m_mainMenu;
		m_mainMenu = nullptr;
	}

	if (m_spaceShip)
	{
		delete m_spaceShip;
		m_spaceShip = nullptr;
	}

	if (m_planet)
	{
		delete m_planet;
		m_planet = nullptr;
	}

	if (m_shipGuns)
	{
		delete m_shipGuns;
		m_shipGuns = nullptr;
	}
	
	if (m_shipGun1_node)
	{
		delete m_shipGun1_node;
		m_shipGun1_node = nullptr;
	}

	if (m_shipGun2_node)
	{
		delete m_shipGun1_node;
		m_shipGun1_node = nullptr;
	}

	if (m_root_node)
	{
		delete m_root_node;
		m_root_node = nullptr;
	}

	if (m_spaceship_node)
	{
		delete m_spaceship_node;
		m_spaceship_node = nullptr;
	}

	if (m_bulletMesh)
	{
		delete m_bulletMesh;
		m_bulletMesh = nullptr;
	}

	for (unsigned int i = 0; i < m_shipBullets.size(); i++)
	{
		delete m_shipBullets[i];
		m_shipBullets[i] = nullptr;
		m_shipBullets.clear();
	}

	for (unsigned int i = 0; i < m_planets.size(); i++)
	{
		delete m_planets[i];
		m_planets[i] = nullptr;
		m_planets.clear();
	}

}

//set up the game and run the main game loop
int GameSystem::playGame(MSG msg, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(renderer->InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(renderer->InitialiseInput()))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(renderer->InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(renderer->InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to Initialise Graphics");
		return 0;
	}

	//run the main menu
	m_mainMenu->StartMenu(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext, renderer);

	//set up the main game when menu is done
	SetupLevel();

	//Main game loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//get the deltatime
			float deltaTime = Renderer::time.GetDeltaTime();
			//bool isColliding = m_node1->CheckCollision(m_node2, m_root_node);

			//Get the controller and keyboard input
			GetControllerInput();
			GetKeyboardInput();

			//update the projectiles
			for (unsigned int i = 0; i < m_shipBullets.size(); i++)
			{
				if (m_shipBullets[i]->GetIsFired())
				{
					m_shipBullets[i]->UpdateProjectile(m_root_node);

				}

				//check for collision with the planets
				for (unsigned int j = 0; j < m_planets.size(); j++)
				{
					if (m_shipBullets[i]->CheckCollision(m_planets[j], m_root_node))
					{
						m_planets[j]->RemoveHealth(5);
						//set the bullet back to origin and dont draw it
						m_shipBullets[i]->SetXPos(m_spaceship_node->GetXPos());
						m_shipBullets[i]->SetYPos(m_spaceship_node->GetYPos());
						m_shipBullets[i]->SetZPos(m_spaceship_node->GetZPos());
						m_shipBullets[i]->setCanCollide(false);
						m_shipBullets[i]->setCanDraw(false);
						m_shipBullets[i]->SetIsFired(false);
					}
				}

				//check for collision with the planets
				for (unsigned int j = 0; j < m_planet1Enemies.size(); j++)
				{
					if (m_shipBullets[i]->CheckCollision(m_planet1Enemies[j], m_root_node))
					{
						m_planet1Enemies[j]->RemoveHealth(5);
						//set the bullet back to origin and dont draw it
						m_shipBullets[i]->SetXPos(m_spaceship_node->GetXPos());
						m_shipBullets[i]->SetYPos(m_spaceship_node->GetYPos());
						m_shipBullets[i]->SetZPos(m_spaceship_node->GetZPos());
						m_shipBullets[i]->setCanCollide(false);
						m_shipBullets[i]->setCanDraw(false);
						m_shipBullets[i]->SetIsFired(false);
					}
				}

			}

			for (unsigned int i = 0; i < m_planet1Enemies.size(); i++)
			{
				m_planet1Enemies[i]->UpdateEnemy(m_spaceship_node);
			}

			for (unsigned int i = 0; i < m_planet2Enemies.size(); i++)
			{
				m_planet2Enemies[i]->UpdateEnemy(m_spaceship_node);
			}
			//set the camera to follow the spaceship
			Renderer::camera->CameraFollow(m_spaceship_node->GetXPos(), m_spaceship_node->GetYPos(), m_spaceship_node->GetZPos());
			
			//update the planets
			for (unsigned int i = 0; i < m_planets.size(); i++)
			{
				m_planets[i]->UpdatePlanet(m_spaceship_node);
			}

			//render the scene
			renderer->RenderFrame(m_root_node, m_planets);


		}
	}

	//release all object before exiting
	renderer->ShutdownD3D();

	return 0;
}

//set up the main game level
void GameSystem::SetupLevel()
{
	//create spaceship mesh
	m_spaceShip = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_spaceShip->LoadObjModel("assets/spaceship.obj");
	m_spaceShip->AddTexture("assets/Spaceship_D.bmp");

	//create the ship guns mesh
	m_shipGuns = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_shipGuns->LoadObjModel("assets/ShipGun.obj");
	m_shipGuns->AddTexture("assets/Spaceship_D.bmp");
	
	//create the planet mesh
	m_planet = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_planet->LoadObjModel("assets/sphere.obj");
	m_planet->AddTexture("assets/planetTexture.png");

	//Create the enemy mesh
	m_enemyMesh = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_enemyMesh->LoadObjModel("assets/Enemy.obj");
	m_enemyMesh->AddTexture("assets/enemytexture.png");

	//create the scene nodes
	m_root_node = new Scene_Node();
	m_spaceship_node = new Scene_Node();
	m_shipGun1_node = new Scene_Node();
	m_shipGun2_node = new Scene_Node();

	//give the scene nodes their respective meshes
	m_spaceship_node->SetModel(m_spaceShip);
	m_shipGun1_node->SetModel(m_shipGuns);
	m_shipGun2_node->SetModel(m_shipGuns);
	m_root_node->AddChildNode(m_spaceship_node);
	m_spaceship_node->AddChildNode(m_shipGun1_node);
	m_spaceship_node->AddChildNode(m_shipGun2_node);

	//set the start pos of the plaer ship
	m_spaceship_node->SetScale(0.1f);
	m_spaceship_node->SetXPos(0.0f);
	m_spaceship_node->SetZPos(10.0f);

	//Position the guns in the correct position
	m_shipGun1_node->SetXPos(-15.0f);
	m_shipGun1_node->SetYPos(-5.0f);
	m_shipGun1_node->SetZPos(35.0f);
	m_shipGun1_node->setCanCollide(false);
	m_shipGun2_node->SetXPos(15.0f);
	m_shipGun2_node->SetYPos(-5.0f);
	m_shipGun2_node->SetZPos(35.0f);
	m_shipGun2_node->SetZAngle(180.0f);
	m_shipGun2_node->setCanCollide(false);

	//create the bullet mesh
	m_bulletMesh = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_bulletMesh->LoadObjModel("assets/Bullet.obj");
	m_bulletMesh->AddTexture("assets/texture.bmp");

	//pool the projectiles
	for (int i = 0; i < NUM_OF_BULLETS; i++)
	{
		m_shipBullets.push_back(new Projectile(2.0f));
	}
	
	//give the projectiles the bullet mesh and make is so they arent rendered
	for (unsigned int i = 0; i < m_shipBullets.size(); i++)
	{
		m_shipBullets[i]->SetModel(m_bulletMesh);
		m_root_node->AddChildNode(m_shipBullets[i]);
		m_shipBullets[i]->setCanCollide(false);
		m_shipBullets[i]->setCanDraw(false);
	}

	//create the planets
	m_planets.push_back(new Planet(PLANET_0_HEALTH, PLANET_0_SIZE, PLANET_0_X_POS, PLANET_0_Y_POS, PLANET_0_Z_POS));
	m_planets.push_back(new Planet(PLANET_1_HEALTH, PLANET_1_SIZE, PLANET_1_X_POS, PLANET_1_Y_POS, PLANET_1_Z_POS));
	//m_planets.push_back(new Planet(100, 20, 100, 10, 300));
	//m_planets.push_back(new Planet(100, 20, 100, 10, 300));

	//give the planets their mesh
	for (unsigned int i = 0; i < m_planets.size(); i++)
	{
		m_planets[i]->SetModel(m_planet);
		m_root_node->AddChildNode(m_planets[i]);
	}

	float enemyxPos = PLANET_0_X_POS - 100;
	float enemyyPos = PLANET_0_Y_POS;
	float enemyzPos = PLANET_0_Z_POS;
	for (int i = 0; i < NUM_OF_ENEMIES; i++)
	{
		m_planet1Enemies.push_back(new Enemy(ENEMY_HEALTH, enemyxPos, enemyyPos, enemyzPos, 0.2));
		
		enemyzPos += 20;
	}

	for (unsigned int i = 0; i < m_planet1Enemies.size(); i++)
	{
		m_planet1Enemies[i]->SetModel(m_enemyMesh);
		m_root_node->AddChildNode(m_planet1Enemies[i]);
	}

	enemyxPos = PLANET_1_X_POS + 100;
	enemyyPos = PLANET_1_Y_POS;
	enemyzPos = PLANET_1_Z_POS;
	for (int i = 0; i < NUM_OF_ENEMIES; i++)
	{
		m_planet2Enemies.push_back(new Enemy(ENEMY_HEALTH, enemyxPos, enemyyPos, enemyzPos, 0.2));

		enemyzPos += 30;
	}

	for (unsigned int i = 0; i < m_planet2Enemies.size(); i++)
	{
		m_planet2Enemies[i]->SetModel(m_enemyMesh);
		m_root_node->AddChildNode(m_planet2Enemies[i]);
	}
}

//Get the keyboard input
void GameSystem::GetKeyboardInput()
{
	if (renderer->IsKeyPressed(DIK_1))
	{
		
	}

	if (renderer->IsKeyPressed(DIK_2))
	{
		
	}

	if (renderer->IsKeyPressed(DIK_W))
	{
		m_spaceship_node->MoveForward(0.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_S))
	{
		m_spaceship_node->MoveForward(-0.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_D))
	{
		m_spaceship_node->UpdateYangle(1.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_A))
	{
		m_spaceship_node->UpdateYangle(-1.5f, m_root_node);
	}

	//change the direction the camera is facing
	if (renderer->IsKeyPressed(DIK_UP))
	{
		Renderer::camera->SetIsForward(true);
		Renderer::camera->SetIsBackward(false);
		Renderer::camera->SetIsRight(false);
		Renderer::camera->SetIsLeft(false);
	}

	if (renderer->IsKeyPressed(DIK_DOWN))
	{
		Renderer::camera->SetIsForward(false);
		Renderer::camera->SetIsBackward(true);
		Renderer::camera->SetIsRight(false);
		Renderer::camera->SetIsLeft(false);
	}

	if (renderer->IsKeyPressed(DIK_RIGHT))
	{
		Renderer::camera->SetIsForward(false);
		Renderer::camera->SetIsBackward(false);
		Renderer::camera->SetIsRight(true);
		Renderer::camera->SetIsLeft(false);
	}

	if (renderer->IsKeyPressed(DIK_LEFT))
	{
		Renderer::camera->SetIsForward(false);
		Renderer::camera->SetIsBackward(false);
		Renderer::camera->SetIsRight(false);
		Renderer::camera->SetIsLeft(true);
	}

	if (renderer->IsKeyPressed(DIK_Q))
	{
		m_spaceship_node->UpdateYPos(0.2f, m_root_node);
	}
	if (renderer->IsKeyPressed(DIK_E))
	{
		m_spaceship_node->UpdateYPos(-0.2f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_N))
	{
		Renderer::camera->Rotate(0.3f, m_spaceship_node->GetXPos(), m_spaceship_node->GetYPos(), m_spaceship_node->GetZPos());
	}

	if (renderer->IsKeyPressed(DIK_M))
	{
		Renderer::camera->Rotate(-0.3f, m_spaceship_node->GetXPos(), m_spaceship_node->GetYPos(), m_spaceship_node->GetZPos());
	}

	m_spaceship_node->UpdateXangle(renderer->mouseCurrState.lY * 0.1f, m_root_node);
	m_spaceship_node->UpdateYangle(renderer->mouseCurrState.lX * 0.1f, m_root_node);

	if (renderer->mouseCurrState.rgbButtons[0])
	{
		if (!m_isMousePressed)
		{
			m_shipBullets[m_bulletNum]->setCanDraw(true);

			m_shipBullets[m_bulletNum]->SetScale(0.1f);
			m_shipBullets[m_bulletNum]->setDirection(m_spaceship_node->GetXDir(), m_spaceship_node->GetYDir(), m_spaceship_node->GetZDir());


			m_shipBullets[m_bulletNum]->SetIsFired(true);
			
			m_shipBullets[m_bulletNum]->SetXPos(m_spaceship_node->GetXPos());
			m_shipBullets[m_bulletNum]->SetYPos(m_spaceship_node->GetYPos());
			m_shipBullets[m_bulletNum]->SetZPos(m_spaceship_node->GetZPos());


			m_bulletNum++;

			if (m_bulletNum == 50)
			{
				m_bulletNum = 0;
			}
		}
		m_isMousePressed = true;

	}

	if (!renderer->mouseCurrState.rgbButtons[0])
	{
		m_isMousePressed = false;

	}

	if (renderer->IsKeyPressed(DIK_SPACE))
	{
		m_spaceship_node->DetachNode(m_shipGun1_node);
	}
}

void GameSystem::GetControllerInput()
{
	//gets the x and y values for the controller thumbsticks
	float leftSticValY = player1->GetState().Gamepad.sThumbLY / 10000.0f;
	float leftSticValX = player1->GetState().Gamepad.sThumbLX / 10000.0f;

	float rightSticValY = player1->GetState().Gamepad.sThumbRY / 10000.0f;
	float rightSticValX = player1->GetState().Gamepad.sThumbRX / 10000.0f;


	if (player1->IsConnected())
	{
		if ((leftSticValY > 10) || (leftSticValY))
		{

			Renderer::camera->Forward(leftSticValY, m_root_node);

		}

		if ((leftSticValX > 10) || (leftSticValX < -10))
		{
			Renderer::camera->Strafe(leftSticValX);

		}

		if ((rightSticValY > 10) || (rightSticValY < -10))
		{

			Renderer::camera->Pitch(rightSticValY);

		}

		if ((rightSticValX > 10) || (rightSticValX < -10))
		{

			//Renderer::camera->Rotate(rightSticValX);

		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			Renderer::camera->Up(0.3f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			Renderer::camera->Up(-0.3f);
		}

		//needs updating
		/*if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			mesh->MoveForward(0.1f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		{
			mesh->MoveForward(-0.1f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
		{
			mesh->UpdateXPos(0.1f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			mesh->UpdateXPos(-0.1f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
		{
			mesh->UpdateYPos(-0.1f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
		{
			mesh->UpdateYPos(0.1f);
		}*/
	}
	else
	{
		printf("\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n");
		//std::cout << "Press Any Key To Exit.";
		//std::cin.get();

	}
}
