#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <dinput.h>
#include <sstream>


#include <DirectXMath.h>
using namespace DirectX;

#include "Camera.h"
#include "CXBOXController.h"
#include "text2D.h"
#include "Mesh.h"
#include "Scene_Node.h"
#include "SkyBox.h"
#include "Time.h"

__declspec(align(16)) class Renderer
{
public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		return _mm_free(p);
	}

	Renderer();
	~Renderer();

	HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitialiseD3D();
	void ShutdownD3D();
	void RenderFrame(Scene_Node* rootNode);
	HRESULT InitialiseGraphics(void);

	HRESULT InitialiseInput();
	void ReadInputState();
	bool IsKeyPressed(unsigned char DI_keycode);

	static Camera* camera;
	static SkyBox* skyBox;
	static Time time;

	static ID3D11Device*           m_pD3DDevice;
	static ID3D11DeviceContext*    m_pImmediateContext;

	DIMOUSESTATE mouseCurrState;

private:
	

	// Rename for each tutorial
	char		m_GameName[100] = "Space Game\0";

	HINSTANCE	m_hInst = NULL;

	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
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
	
	ID3D11BlendState* m_pAlphaBlendEnable;
	ID3D11BlendState* m_pAlphaBlendDisable;

	IDirectInput8* m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	IDirectInputDevice8* m_mouse_device;
	unsigned char m_keyboard_keys_state[256];

	Text2D* text;
	int m_fps = 0;
	stringstream m_FPS;
};

