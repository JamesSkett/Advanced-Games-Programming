#include "Renderer.h"
#include <iostream>

HWND		m_hWnd = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

ID3D11Device*           Renderer::m_pD3DDevice;
ID3D11DeviceContext*    Renderer::m_pImmediateContext;

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
	char Name[100] = "Space Game\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
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
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &viewport);


	return S_OK;
}

void Renderer::ShutdownD3D()
{
	if (camera)
	{
		delete camera;
		camera = nullptr;
	}

	if (m_keyboard_device)
	{
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}

	if (m_direct_input) m_direct_input->Release();

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

void Renderer::RenderFrame(Scene_Node* rootNode)
{

	// Clear the back buffer - choose a colour you like
	float rgba_clear_colour[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);

	m_pImmediateContext->ClearDepthStencilView(m_pzBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ReadInputState();

	XMMATRIX identity, projection, view;

	identity = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1920.0f / 1080.0f, 1.0f, 100.0f);
	view = camera->GetViewMatrix();

	rootNode->Execute(&identity, &view, &projection);

	// RENDER HERE

	/*XMMATRIX identity, projection, view;

	identity = XMMatrixIdentity();
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1920.0f / 1080.0f, 1.0f, 100.0f);
	view = camera->GetViewMatrix();*/

	//mesh->SetYAngle(degrees2);
	//mesh->Lookat_XZ(camera->GetX(), camera->GetY(), camera->GetZ());
	//mesh2->Lookat_XZ(mesh->GetXPos(), mesh->GetYPos(), mesh->GetZPos());

	//camera->CameraFollow(mesh->GetXPos(), mesh->GetYPos(), mesh->GetZPos());
	//camera->LookAt(mesh->GetXPos(), mesh->GetZPos());

	//mesh->Draw(&view, &projection);
	//mesh2->Draw(&view, &projection);

	//bool isColliding = mesh->CheckCollision(mesh2);

	

	// Select which primitive type to use //03-01
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (IsKeyPressed(DIK_ESCAPE)) DestroyWindow(m_hWnd);

	// Display what has just been rendered
	m_pSwapChain->Present(1, 0);
}

HRESULT Renderer::InitialiseGraphics(void)
{
	camera = new Camera(0.0f, 0.0f, 0.0f, 0.0f);
	

	return S_OK;
}

HRESULT Renderer::InitialiseInput()
{
	HRESULT hr;
	ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));

	hr = DirectInput8Create(m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;


	return S_OK;
}

void Renderer::ReadInputState()
{
	HRESULT hr;

	hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_keyboard_device->Acquire();
		}
	}
}

bool Renderer::IsKeyPressed(unsigned char DI_keycode)
{
	return m_keyboard_keys_state[DI_keycode] & 0x80;
}


