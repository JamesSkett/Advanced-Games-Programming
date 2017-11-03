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
	RECT rc = { 0, 0, 640, 480 };
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

	m_2DText = new Text2D("assets/font1.bmp", m_pD3DDevice, m_pImmediateContext);

	return S_OK;
}

void Renderer::ShutdownD3D()
{
	delete mesh;
	mesh = nullptr;

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

float degrees = 0.0f;
float degrees2 = 0.0f;
float z = 5.0f;

void Renderer::RenderFrame(void)
{
	degrees += 0.01f;
	degrees2 += 0.005f;
	z += 0.0001f;

	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);

	m_pImmediateContext->ClearDepthStencilView(m_pzBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	float leftSticValY = player1->GetState().Gamepad.sThumbLY / 1000.0f;
	float leftSticValX = player1->GetState().Gamepad.sThumbLX / 1000.0f;


	if (player1->IsConnected())
	{
		if ((leftSticValY > 10) || (leftSticValY < -10))
		{
			
			camera->Forward(leftSticValY / 10000.0f);
		
		}

		if ((leftSticValX > 10) || (leftSticValX < -10))
		{

			camera->Strafe(-leftSticValX / 10000.0f);

		}

		if (player1->GetState().Gamepad.sThumbRX > 20000)
		{
			camera->Rotate(0.01f);
		}

		if (player1->GetState().Gamepad.sThumbRX < -20000)
		{
			camera->Rotate(-0.01f);
		}

		if (player1->GetState().Gamepad.sThumbRY > 20000)
		{
			camera->Pitch(0.003f);
		}

		if (player1->GetState().Gamepad.sThumbRY < -20000)
		{
			camera->Pitch(-0.003f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			camera->Up(0.003f);
		}

		if (player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			camera->Up(-0.003f);
		}
	}
	else
	{
		std::cout << "\n\tERROR! PLAYER 1 - XBOX 360 Controller Not Found!\n";
		std::cout << "Press Any Key To Exit.";
		std::cin.get();

	}

	// RENDER HERE

	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);


	XMMATRIX transpose, transpose2;
	CONSTANT_BUFFER0 cb0_values;

	XMMATRIX projection, cube1world, cube2world, view;


	//cube1world = XMMatrixRotationY(XMConvertToRadians(degrees));
	//cube1world *= XMMatrixRotationX(XMConvertToRadians(degrees2));
	//cube1world *= XMMatrixTranslation(0, 0, 8);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 640.0f / 480.0f, 1.0f, 100.0f);
	view = camera->GetViewMatrix();
	cb0_values.WorldViewProjection = cube1world * view * projection;

	//transpose = XMMatrixTranspose(cube1world); // model world matrix


	//cb0_values.directional_light_colour = g_directional_light_colour;
	//cb0_values.ambient_light_colour = g_ambient_light_colour;
	//cb0_values.directional_light_vector = XMVector3Transform(g_directional_light_shines_from, transpose);
	//cb0_values.directional_light_vector = XMVector3Normalize(cb0_values.directional_light_vector);

	// upload the new values for the constant buffer
	//m_pImmediateContext->UpdateSubresource(m_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);

	//m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	//m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	//m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);


	mesh->SetScale(0.5f);
	//mesh->SetXAngle(180.0f);


	mesh->Draw(&view, &projection);

	//Draw the vertex buffer to the back buffer
	//m_pImmediateContext->Draw(36, 0);

	//cube2world = XMMatrixRotationX(XMConvertToRadians(degrees));
	//cube2world *= XMMatrixRotationY(XMConvertToRadians(degrees2));
	//cube2world = XMMatrixTranslation(4, 0, 8);
	//cb0_values.WorldViewProjection = cube2world * view * projection;

	//transpose2 = XMMatrixTranspose(cube2world); // model world matrix


	//cb0_values.directional_light_colour = g_directional_light_colour;
	//cb0_values.ambient_light_colour = g_ambient_light_colour;
	//cb0_values.directional_light_vector = XMVector3Transform(g_directional_light_shines_from, transpose2);
	//cb0_values.directional_light_vector = XMVector3Normalize(cb0_values.directional_light_vector);

	// upload the new values for the constant buffer
	//m_pImmediateContext->UpdateSubresource(m_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);

	//m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	// Draw the vertex buffer to the back buffer
	//m_pImmediateContext->Draw(36, 0);

	//Set the vertex buffer //03-01
	UINT stride = sizeof(POS_COL_TEX_NORM_VERTEX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);



	// Select which primitive type to use //03-01
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//m_2DText->AddText("Health", -0.5f, 0.5f, .2);

	//m_2DText->RenderText();

	//m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);
	//m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);
	//m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	


	// Display what has just been rendered
	m_pSwapChain->Present(0, 0);
}

HRESULT Renderer::InitialiseGraphics(void)
{
	HRESULT hr = S_OK;

	mesh = new Mesh(m_pD3DDevice, m_pImmediateContext);
	mesh->LoadObjModel("assets/AK47.obj");

	// Define vertices of a triangle - screen coordinates -1.0 to +1.0
	POS_COL_TEX_NORM_VERTEX vertices[] =
	{
		//Cube 1
		// back face
		{ XMFLOAT3(-1.0f,  1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f,   1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f,   1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f,  -1.0f, 1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		// front face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f,   1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f,   1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f,  -1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		// left face
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f,  1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f),     XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		// right face
		{ XMFLOAT3(1.0f, -1.0f,  1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f,  1.0f, -1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f,  1.0f,  1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f,  1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f,  1.0f, -1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		// bottom face
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),     XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f,  1.0f),      XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f,  1.0f),      XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f,  1.0f),     XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),     XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 1.0f) },

		// top face
		{ XMFLOAT3(1.0f, 1.0f,  1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),       XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f,  1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f,  1.0f),       XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),      XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },

	};

	//Set up and create the vertext buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; //Used by the CPU and GPU
	bufferDesc.ByteWidth = sizeof(POS_COL_TEX_NORM_VERTEX) * 72; //Total size of buffer
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //Use as vertex buffer
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE; //Allow CPU access
	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer); //Create the buffer

	if (FAILED(hr))
	{
		return hr;
	}

	// Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	// Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 112;  // Must Be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer0);

	if (FAILED(hr))
	{
		return hr;
	}

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, "assets/texture.bmp", NULL, NULL, &m_pTexture0, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	// Lock the buffer to allow writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));


	//unlock the data
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	//Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)// check for shader compliation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) //dont fail if error is just a warning
		{
			return hr;
		};
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)// check for shader compliation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) //dont fail if error is just a warning
		{
			return hr;
		};
	}

	//Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVertexShader);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPixelShader);

	if (FAILED(hr))
	{
		return hr;
	}

	// Set the shader objects as active
	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

	// Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,    0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	camera = new Camera(0.0f, 0.0f, -0.5f, 0.0f);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	return S_OK;
}