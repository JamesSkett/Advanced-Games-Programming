#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h>
//#define _XM_NO_INTRINSICS
//#define XM_NO_ALIGNMENT
//#include <xnamath.h>

#include <DirectXMath.h>
using namespace DirectX;

#include "Camera.h"
#include "CXBOXController.h"
#include "text2D.h"
#include "Mesh.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitialiseD3D();
	void ShutdownD3D();
	void RenderFrame(void);
	HRESULT InitialiseGraphics(void);

	HRESULT InitialiseInput();
	void ReadInputState();
	bool IsKeyPressed(unsigned char DI_keycode);

	void GetKeyboardInput();

	static Camera* camera;
	
	Mesh* mesh;
	Mesh* mesh2;

private:
	

	// Rename for each tutorial
	char		m_GameName[100] = "Space Game\0";

	HINSTANCE	m_hInst = NULL;

	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           m_pD3DDevice = NULL;
	ID3D11DeviceContext*    m_pImmediateContext = NULL;
	IDXGISwapChain*         m_pSwapChain = NULL;

	ID3D11RenderTargetView* m_pBackBufferRTView = NULL;

	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer0;

	ID3D11DepthStencilView* m_pzBuffer;
	ID3D11ShaderResourceView* m_pTexture0;
	ID3D11SamplerState* m_pSampler0;

	XMVECTOR g_directional_light_shines_from;
	XMVECTOR g_directional_light_colour;
	XMVECTOR g_ambient_light_colour;

	CXBOXController* player1 = new CXBOXController(1);

	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	unsigned char m_keyboard_keys_state[256];


	
};

