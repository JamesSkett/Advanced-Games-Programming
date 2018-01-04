#include "GameSystem.h"

#include <thread>

Math* GameSystem::math;

GameSystem::GameSystem()
{
	renderer = new Renderer;
}


GameSystem::~GameSystem()
{
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
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

	/*if (cameraMesh)
	{
		delete cameraMesh;
		cameraMesh = nullptr;
	}*/

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

	if (m_planet_node)
	{
		delete m_planet_node;
		m_planet_node = nullptr;
	}

	/*if (m_camera_node)
	{
		delete m_camera_node;
		m_camera_node = nullptr;
	}*/
}

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
			float deltaTime = Renderer::time.GetDeltaTime();
			//bool isColliding = m_node1->CheckCollision(m_node2, m_root_node);

			
			GetControllerInput();
			GetKeyboardInput();

			for (unsigned int i = 0; i < m_shipBullets.size(); i++)
			{
				if (m_shipBullets[i]->GetIsFired())
				{
					m_shipBullets[i]->UpdateProjectile(m_root_node);

				}

				
			}


			//Renderer::camera->SetX(m_camera_node->GetXPos());
			//Renderer::camera->SetY(m_camera_node->GetYPos());
			Renderer::camera->CameraFollow(m_spaceship_node->GetXPos(), m_spaceship_node->GetYPos(), m_spaceship_node->GetZPos());
			

			renderer->RenderFrame(m_root_node);


		}
	}

	renderer->ShutdownD3D();

	return 0;
}

void GameSystem::SetupLevel()
{
	m_spaceShip = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_spaceShip->LoadObjModel("assets/spaceship.obj");
	m_spaceShip->AddTexture("assets/Spaceship_D.bmp");

	m_shipGuns = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	
	m_shipGuns->LoadObjModel("assets/ShipGun.obj");
	m_shipGuns->AddTexture("assets/Spaceship_D.bmp");
	
	
	m_planet = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_planet->LoadObjModel("assets/sphere.obj");
	m_planet->AddTexture("assets/planetTexture.png");

	m_root_node = new Scene_Node();
	m_spaceship_node = new Scene_Node();
	m_planet_node = new Scene_Node();
	m_shipGun1_node = new Scene_Node();
	m_shipGun2_node = new Scene_Node();

	/*text = new Text2D("assets/font1.bmp", Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	text->AddText("HELLO WORLD!", 0.0f, 0.0f, 0.2f);
	text->RenderText();*/


	m_spaceship_node->SetModel(m_spaceShip);

	m_planet_node->SetModel(m_planet);

	m_shipGun1_node->SetModel(m_shipGuns);
	m_shipGun2_node->SetModel(m_shipGuns);

	m_root_node->AddChildNode(m_spaceship_node);
	m_root_node->AddChildNode(m_planet_node);

	m_spaceship_node->AddChildNode(m_shipGun1_node);
	m_spaceship_node->AddChildNode(m_shipGun2_node);


	m_spaceship_node->SetScale(0.1f);
	m_spaceship_node->SetXPos(0.0f);
	m_spaceship_node->SetZPos(10.0f);

	m_planet_node->SetZPos(100.1f);
	m_planet_node->SetXPos(100.0f);
	m_planet_node->SetScale(20.0f);

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

	m_bulletMesh = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	m_bulletMesh->LoadObjModel("assets/Bullet.obj");
	m_bulletMesh->AddTexture("assets/texture.bmp");

	for (int i = 0; i < NUM_OF_BULLETS; i++)
	{
		m_shipBullets.push_back(new Projectile(2.0f));
	}
	
	for (unsigned int i = 0; i < m_shipBullets.size(); i++)
	{
		m_shipBullets[i]->SetModel(m_bulletMesh);
		m_root_node->AddChildNode(m_shipBullets[i]);
		m_shipBullets[i]->setCanCollide(false);
		m_shipBullets[i]->setCanDraw(false);
	}

	//XMMATRIX identity = XMMatrixIdentity();

	//m_root_node->UpdateCollisionTree(&identity, 1.0f);
}

void GameSystem::GetKeyboardInput()
{
	if (renderer->IsKeyPressed(DIK_1))
	{
		m_planet_node->setCanCollide(true);
		m_planet_node->setCanDraw(true);
	}

	if (renderer->IsKeyPressed(DIK_2))
	{
		m_planet_node->setCanCollide(false);
		m_planet_node->setCanDraw(false);
	}

	if (renderer->IsKeyPressed(DIK_W))
	{
		m_spaceship_node->MoveForward(0.1f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_S))
	{
		m_spaceship_node->MoveForward(-0.1f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_D))
	{
		//m_node1->UpdateYangle(0.5f, m_root_node);
		m_spaceship_node->UpdateYangle(1.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_A))
	{
		//m_node1->UpdateYangle(-0.5f, m_root_node);
		m_spaceship_node->UpdateYangle(-1.5f, m_root_node);
	}

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

	//Renderer::camera->Strafe(-renderer->mouseCurrState.lX);
	//Renderer::camera->Rotate(-renderer->mouseCurrState.lX, m_node1->GetXPos(), m_node1->GetYPos(), m_node1->GetZPos());

	m_spaceship_node->UpdateXangle(renderer->GetMousY() * 0.1f, m_root_node);
	m_spaceship_node->UpdateYangle(renderer->GetMousX() * 0.1f, m_root_node);

	if (renderer->mouseCurrState.rgbButtons[0])
	{
		if (!m_isMousePressed)
		{
			m_shipBullets[m_bulletNum]->setCanDraw(true);

			m_shipBullets[m_bulletNum]->SetScale(0.1f);
			m_shipBullets[m_bulletNum]->setDirection(m_spaceship_node->GetXDir(), m_spaceship_node->GetYDir(), m_spaceship_node->GetZDir());

			m_shipBullets[m_bulletNum]->SetIsFired(true);
			if (m_leftGun)
			{
				m_shipBullets[m_bulletNum]->SetXPos(m_spaceship_node->GetXPos() - 2);
				m_shipBullets[m_bulletNum]->SetYPos(m_spaceship_node->GetYPos());
				m_shipBullets[m_bulletNum]->SetZPos(m_spaceship_node->GetZPos());
				m_rightGun = true;
				m_leftGun = false;
			}
			else if (m_rightGun)
			{
				m_shipBullets[m_bulletNum]->SetXPos(m_spaceship_node->GetXPos() + 2);
				m_shipBullets[m_bulletNum]->SetYPos(m_spaceship_node->GetYPos());
				m_shipBullets[m_bulletNum]->SetZPos(m_spaceship_node->GetZPos());
				m_rightGun = false;
				m_leftGun = true;
			}
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
