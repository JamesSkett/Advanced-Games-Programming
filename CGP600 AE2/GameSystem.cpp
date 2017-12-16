#include "GameSystem.h"

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

	if (mesh)
	{
		delete mesh;
		mesh = nullptr;
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

	

	if (m_root_node)
	{
		delete m_root_node;
		m_root_node = nullptr;
	}

	if (m_node1)
	{
		delete m_node1;
		m_node1 = nullptr;
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

			renderer->RenderFrame(m_root_node);


		}
	}

	renderer->ShutdownD3D();

	return 0;
}

void GameSystem::SetupLevel()
{
	mesh = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	mesh->LoadObjModel("assets/spaceship.obj");
	
	mesh2 = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	mesh2->LoadObjModel("assets/Sphere.obj");

	//cameraMesh = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	//cameraMesh->LoadObjModel("assets/cube.obj");

	mesh->AddTexture("assets/Spaceship_D.bmp");
	mesh2->AddTexture("assets/texture.bmp");
	//cameraMesh->AddTexture("assets/texture.bmp");

	m_root_node = new Scene_Node();
	m_node1 = new Scene_Node();
	m_node2 = new Scene_Node();
	//m_camera_node = new Scene_Node();
	//g_node3 = new Scene_Node();

	m_node1->SetModel(mesh);
	m_node2->SetModel(mesh2);
	//m_camera_node->SetModel(cameraMesh);
	//g_node3->SetModel(mesh);

	m_root_node->AddChildNode(m_node1);
	m_root_node->AddChildNode(m_node2);
	//m_root_node->AddChildNode(m_camera_node);
	//m_node1->AddChildNode(m_node2);
	//g_node2->AddChildNode(g_node3);

	m_node1->SetScale(0.1f);
	m_node2->SetScale(0.3f);
	m_node1->SetZPos(5.0f);
	m_node2->SetZPos(5.0f);
	m_node2->SetXPos(2.0f);
	m_node1->SetXPos(-5.0f);
	//m_camera_node->SetScale(0.1f);

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
		m_node1->MoveForward(0.1f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_S))
	{
		m_node1->MoveForward(-0.1f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_UP))
	{
		Renderer::camera->Forward(0.1f, m_root_node);

		//// set camera node to the position of the camera
		//m_camera_node->SetXPos(Renderer::camera->GetX());
		//m_camera_node->SetYPos(Renderer::camera->GetY());
		//m_camera_node->SetZPos(Renderer::camera->GetZ());

		//XMMATRIX identity = XMMatrixIdentity();

		//// update tree to reflect new camera position
		//m_root_node->UpdateCollisionTree(&identity, 1.0);

		//if (m_camera_node->CheckCollision(m_root_node) == true)
		//{
		//	// if there is a collision, restore camera and camera node positions
		//	Renderer::camera->Forward(-0.3f);
		//	m_camera_node->SetXPos(Renderer::camera->GetX()); //15
		//	m_camera_node->SetYPos(Renderer::camera->GetY());//15
		//	m_camera_node->SetZPos(Renderer::camera->GetZ());//15

		//}

	}

	if (renderer->IsKeyPressed(DIK_DOWN))
	{
		Renderer::camera->Forward(-0.1f, m_root_node);

		//// set camera node to the position of the camera
		//m_camera_node->SetXPos(Renderer::camera->GetX());
		//m_camera_node->SetYPos(Renderer::camera->GetY());
		//m_camera_node->SetZPos(Renderer::camera->GetZ());

		//XMMATRIX identity = XMMatrixIdentity();

		//// update tree to reflect new camera position
		//m_root_node->UpdateCollisionTree(&identity, 1.0);

		//if (m_camera_node->CheckCollision(m_root_node) == true)
		//{
		//	// if there is a collision, restore camera and camera node positions
		//	Renderer::camera->Forward(0.7f);
		//	m_camera_node->SetXPos(Renderer::camera->GetX()); //15
		//	m_camera_node->SetYPos(Renderer::camera->GetY());//15
		//	m_camera_node->SetZPos(Renderer::camera->GetZ());//15

		//}
	}

	if (renderer->IsKeyPressed(DIK_D))
	{
		m_node1->UpdateYangle(0.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_A))
	{
		m_node1->UpdateYangle(-0.5f, m_root_node);
	}

	if (renderer->IsKeyPressed(DIK_RIGHT))
	{
		Renderer::camera->Strafe(-0.1f);

		//// set camera node to the position of the camera
		//m_camera_node->SetXPos(Renderer::camera->GetX());
		//m_camera_node->SetYPos(Renderer::camera->GetY());
		//m_camera_node->SetZPos(Renderer::camera->GetZ());

		//XMMATRIX identity = XMMatrixIdentity();

		//// update tree to reflect new camera position
		//m_root_node->UpdateCollisionTree(&identity, 1.0);

		//if (m_camera_node->CheckCollision(m_root_node) == true)
		//{
		//	// if there is a collision, restore camera and camera node positions
		//	Renderer::camera->Strafe(0.7f);
		//	m_camera_node->SetXPos(Renderer::camera->GetX()); //15
		//	m_camera_node->SetYPos(Renderer::camera->GetY());//15
		//	m_camera_node->SetZPos(Renderer::camera->GetZ());//15

		//}
	}

	if (renderer->IsKeyPressed(DIK_LEFT))
	{
		Renderer::camera->Strafe(0.1f);

		//// set camera node to the position of the camera
		//m_camera_node->SetXPos(Renderer::camera->GetX());
		//m_camera_node->SetYPos(Renderer::camera->GetY());
		//m_camera_node->SetZPos(Renderer::camera->GetZ());

		//XMMATRIX identity = XMMatrixIdentity();

		//// update tree to reflect new camera position
		//m_root_node->UpdateCollisionTree(&identity, 1.0);

		//if (m_camera_node->CheckCollision(m_root_node) == true)
		//{
		//	// if there is a collision, restore camera and camera node positions
		//	Renderer::camera->Strafe(-0.7f);
		//	m_camera_node->SetXPos(Renderer::camera->GetX()); //15
		//	m_camera_node->SetYPos(Renderer::camera->GetY());//15
		//	m_camera_node->SetZPos(Renderer::camera->GetZ());//15

		//}
	}

	if (renderer->IsKeyPressed(DIK_Q))
	{
		m_node1->UpdateYPos(0.2f, m_root_node);
	}
	if (renderer->IsKeyPressed(DIK_E))
	{
		m_node1->UpdateYPos(-0.2f, m_root_node);
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

			mesh->UpdateXAngle(leftSticValY);

		}

		if ((leftSticValX > 10) || (leftSticValX < -10))
		{

			mesh->UpdateYAngle(leftSticValX);

		}

		if ((rightSticValY > 10) || (rightSticValY < -10))
		{

			Renderer::camera->Pitch(rightSticValY);

		}

		if ((rightSticValX > 10) || (rightSticValX < -10))
		{

			Renderer::camera->Rotate(rightSticValX);

		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			Renderer::camera->Up(0.3f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			Renderer::camera->Up(-0.3f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
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
		}
	}
	else
	{
		printf("\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n");
		//std::cout << "Press Any Key To Exit.";
		//std::cin.get();

	}
}