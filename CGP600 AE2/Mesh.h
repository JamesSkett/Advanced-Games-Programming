#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include "objfilemodel.h"

class Mesh
{
public:
	Mesh(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext);
	~Mesh();

	std::vector <XMFLOAT3> verticiesPos;
	std::vector <XMFLOAT4> verticiesCol;
	std::vector <XMFLOAT2> verticiesUV;

	int LoadObjModel(char* fileName);

private:
	ID3D11Device*        m_pD3D11Device;
	ID3D11DeviceContext* m_pImmediateContext;

	ObjFileModel*		 m_pObject;
	ID3D11VertexShader*  m_pVShader;
	ID3D11PixelShader*	 m_pPShader;
	ID3D11InputLayout*	 m_InputLayout;
	ID3D11Buffer*		 m_pConstantBuffer;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;
};

