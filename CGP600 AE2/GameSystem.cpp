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

	if (mesh2)
	{
		delete mesh2;
		mesh2 = nullptr;
	}

	/*if (cameraMesh)
	{
		delete cameraMesh;
		cameraMesh = nullptr;
	}*/

	for (int i = 0; i < m_shipGuns.size(); i++)
	{
		delete m_shipGuns[i];
		m_shipGuns[i] = nullptr;
		m_shipGuns.clear();
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

	if (m_node2)
	{
		delete m_node2;
		m_node2 = nullptr;
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


	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			
			//bool isColliding = m_node1->CheckCollision(m_node2, m_root_node);

			
			GetControllerInput();
			GetKeyboardInput();

			//
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

	//create gun meshes
	for (int i = 0; i < NUMBER_OF_GUNS; i++)
	{
		m_shipGuns.push_back(new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext));
	}

	//set the gun object and texture
	for (int i = 0; i < m_shipGuns.size(); i++)
	{
		m_shipGuns[i]->LoadObjModel("assets/ShipGun.obj");
		m_shipGuns[i]->AddTexture("assets/Spaceship_D.bmp");
	}
	
	mesh2 = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	mesh2->LoadObjModel("assets/sphere.obj");
	mesh2->AddTexture("assets/texture.bmp");

	m_root_node = new Scene_Node();
	m_spaceship_node = new Scene_Node();
	m_node2 = new Scene_Node();
	m_shipGun1_node = new Scene_Node();
	m_shipGun2_node = new Scene_Node();

	text = new Text2D("assets/font1.bmp", Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	text->AddText("HELLO WORLD!", 0.0f, 0.0f, 0.2f);
	text->RenderText();


	m_spaceship_node->SetModel(m_spaceShip);
	m_node2->SetModel(mesh2);
	m_shipGun1_node->SetModel(m_shipGuns[FRONT_LEFT_GUN]);
	m_shipGun2_node->SetModel(m_shipGuns[FRONT_RIGHT_GUN]);

	m_root_node->AddChildNode(m_spaceship_node);
	m_root_node->AddChildNode(m_node2);
	m_spaceship_node->AddChildNode(m_shipGun1_node);
	m_spaceship_node->AddChildNode(m_shipGun2_node);


	m_spaceship_node->SetScale(0.1f);
	m_node2->SetScale(1.0f);
	m_spaceship_node->SetZPos(10.0f);
	m_node2->SetZPos(-3.1f);
	m_node2->SetXPos(7.0f);
	m_spaceship_node->SetXPos(0.0f);

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
		m_shipBullets.push_back(new Projectile(0.00001f));
	}
	
	for (int i = 0; i < m_shipBullets.size() / 2; i++)
	{
		m_shipBullets[i]->SetModel(m_bulletMesh);
		m_shipGun1_node->AddChildNode(m_shipBullets[i]);
	}

	for (int i = 50; i < m_shipBullets.size() / 2; i++)
	{
		m_shipBullets[i]->SetModel(m_bulletMesh);
		m_shipGun2_node->AddChildNode(m_shipBullets[i]);
	}

	//XMMATRIX identity = XMMatrixIdentity();

	//m_root_node->UpdateCollisionTree(&identity, 1.0f);
}

void GameSystem::GetKeyboardInput()
{
	if (renderer->IsKeyPressed(DIK_1))
	{
		m_node2->setCanCollide(true);
		m_node2->setCanDraw(true);
	}

	if (renderer->IsKeyPressed(DIK_2))
	{
		m_node2->setCanCollide(false);
		m_node2->setCanDraw(false);
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

	m_spaceship_node->UpdateXangle(renderer->mouseCurrState.lY * 0.1f, m_root_node);
	m_spaceship_node->UpdateYangle(renderer->mouseCurrState.lX * 0.1f, m_root_node);

	if (renderer->mouseCurrState.rgbButtons[0])
	{
		if (!isMousePressed)
		{
			xyz bulletPos = { m_spaceship_node->GetXPos() + m_shipGun1_node->GetXPos() + m_shipBullets[bulletNum]->GetXPos(),
							  m_spaceship_node->GetYPos() + m_shipGun1_node->GetYPos() + m_shipBullets[bulletNum]->GetYPos(),
							  m_spaceship_node->GetZPos() + m_shipGun1_node->GetZPos() + m_shipBullets[bulletNum]->GetZPos() };

			m_shipGun1_node->DetachNode(m_shipBullets[bulletNum]);
			m_root_node->AddChildNode(m_shipBullets[bulletNum]);
			m_shipBullets[bulletNum]->SetIsFired(true);

			

			float bulletScale = m_spaceShip->GetScale() - m_shipGun1_node->GetScale();

			m_shipBullets[bulletNum]->SetXPos(bulletPos.x);
			m_shipBullets[bulletNum]->SetYPos(bulletPos.y);
			m_shipBullets[bulletNum]->SetZPos(bulletPos.z);
			m_shipBullets[bulletNum]->SetScale(bulletScale);

			thread shootBullet([=]
			{
				if (!m_shipBullets[bulletNum]->UpdateProjectile(m_root_node))
				{
					m_root_node->DetachNode(m_shipBullets[bulletNum]);
					m_shipGun1_node->AddChildNode(m_shipBullets[bulletNum]);
				}
			});
			shootBullet.detach();

			bulletNum++;
		}
		isMousePressed = true;

	}

	if (!renderer->mouseCurrState.rgbButtons[0])
	{
		isMousePressed = false;

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
