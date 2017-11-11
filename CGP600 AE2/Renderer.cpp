#include "Renderer.h"
#include <iostream>

HWND		m_hWnd = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Camera* Renderer::camera;

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

HRESULT Renderer::InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Give your app window your own name
	char Name[100] = "Hello World\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;
	
	// Create window
	m_hInst = hInstance;
	RECT rc = { 0, 0, 1920, 1080 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(Name, m_GameName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!m_hWnd)
		return E_FAIL;

	ShowWindow(m_hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(m_hWnd);
			return 0;
		}
		else if (wParam == 0x41)
		{
			Renderer::camera->Strafe(1.0f);
		}
		else if (wParam == 0x44)
		{
			Renderer::camera->Strafe(-1.0f);
		}
		else if (wParam == 0x57)
		{
			Renderer::camera->Forward(0.3f);
		}
		else if (wParam == 0x53)
		{
			Renderer::camera->Forward(-0.3f);
		}
		else if (wParam == VK_LEFT)
		{
			Renderer::camera->Rotate(2.0f);
		}
		else if (wParam == VK_RIGHT)
		{
			Renderer::camera->Rotate(-2.0f);
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT Renderer::InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain,
			&m_pD3DDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&m_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	// Create the z buffer texture
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* pZBufferTexture;
	hr = m_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//Create the z buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &m_pzBuffer);
	pZBufferTexture->Release();

	// Set the render target view
	m_pImmediateContext->OMSetRenderTargets(1, &m_pBackBufferRTView, m_pzBuffer);



	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &viewport);


	return S_OK;
}

void Renderer::ShutdownD3D()
{
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

	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pInputLayout)  m_pInputLayout->Release();
	if (m_pVertexShader) m_pVertexShader->Release();
	if (m_pPixelShader)  m_pPixelShader->Release();
	if (m_pTexture0)     m_pTexture0->Release();
	if (m_pSampler0)     m_pSampler0->Release();
		
	if (m_pSwapChain)        m_pSwapChain->Release();
	if (m_pConstantBuffer0)  m_pConstantBuffer0->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pD3DDevice)        m_pD3DDevice->Release();
}


void Renderer::RenderFrame(void)
{

	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);

	m_pImmediateContext->ClearDepthStencilView(m_pzBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float leftSticValY = player1->GetState().Gamepad.sThumbLY / 1000.0f;
	float leftSticValX = player1->GetState().Gamepad.sThumbLX / 1000.0f;

	float rightSticValY = player1->GetState().Gamepad.sThumbRY / 1000.0f;
	float rightSticValX = player1->GetState().Gamepad.sThumbRX / 1000.0f;


	if (player1->IsConnected())
	{
		if ((leftSticValY > 10) || (leftSticValY < -10))
		{
			
			mesh->UpdateXAngle(leftSticValY / 10.0f);
		
		}

		if ((leftSticValX > 10) || (leftSticValX < -10))
		{

			mesh->UpdateYAngle(leftSticValX / 10.0f);

		}

		if ((rightSticValY > 10) || (rightSticValY < -10))
		{

			camera->Pitch(rightSticValY / 10.0f);

		}

		if ((rightSticValX > 10) || (rightSticValX < -10))
		{

			camera->Rotate(rightSticValX / 10.0f);

		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			camera->Up(0.3f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			camera->Up(-0.3f);
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
		std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
		std::cout << "Press Any Key To Exit.";
		std::cin.get();

	}

	// RENDER HERE

	XMMATRIX projection, view;

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 640.0f / 480.0f, 1.0f, 100.0f);
	view = camera->GetViewMatrix();

	//mesh->SetYAngle(degrees2);
	//mesh->Lookat_XZ(camera->GetX(), camera->GetY(), camera->GetZ());
	mesh2->Lookat_XZ(mesh->GetXPos(), mesh->GetYPos(), mesh->GetZPos());

	//camera->CameraFollow(mesh->GetXPos(), mesh->GetYPos(), mesh->GetZPos());
	//camera->LookAt(mesh->GetXPos(), mesh->GetZPos());

	mesh->Draw(&view, &projection);
	mesh2->Draw(&view, &projection);

	bool isColliding = mesh->CheckCollision(mesh2);

	// Select which primitive type to use //03-01
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Display what has just been rendered
	m_pSwapChain->Present(1, 0);
}

HRESULT Renderer::InitialiseGraphics(void)
{
	mesh = new Mesh(m_pD3DDevice, m_pImmediateContext);
	mesh->LoadObjModel("assets/Sphere.obj");
	mesh->SetScale(0.1f);
	mesh->SetZPos(10.0f);

	mesh2 = new Mesh(m_pD3DDevice, m_pImmediateContext);
	mesh2->LoadObjModel("assets/Sphere.obj");
	mesh2->SetScale(0.2f);
	mesh2->SetXPos(2.0f);
	mesh2->SetYPos(3.0f);
	mesh2->SetZPos(4.0f);


	mesh->AddTexture("assets/Spaceship_D.bmp");
	mesh2->AddTexture("assets/texture.bmp");

	camera = new Camera(0.0f, 0.0f, -0.5f, 0.0f);

	return S_OK;
}