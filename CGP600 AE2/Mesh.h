#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include "objfilemodel.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
};

class Mesh
{
public:
	Mesh(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext);
	~Mesh();

	int LoadObjModel(char* fileName);

	void Draw(XMMATRIX* view, XMMATRIX* projection);

	//Setters
	void SetXPos(float x_pos);
	void SetYPos(float y_pos);
	void SetZPos(float z_pos);
	void SetXAngle(float x_angle);
	void SetYAngle(float y_angle);
	void SetZAngle(float z_angle);
	void SetScale(float scale);

	//Getters
	float GetXPos();
	float GetYPos();
	float GetZPos();
	float GetXAngle();
	float GetYAndle();
	float GetZAngle();
	float GetScale();

	//use these to move scale and rotate object
	void UpdateXPos(float distance);
	void UpdateYPos(float distance);
	void UpdateZPos(float distance);
	void UpdateXAngle(float angle);
	void UpdateYAngle(float angle);
	void UpdateZAngle(float angle);
	void UpdateScale(float scale);



private:
	ID3D11Device*        m_pD3D11Device;
	ID3D11DeviceContext* m_pImmediateContext;

	ObjFileModel*		 m_pObject;
	ID3D11VertexShader*  m_pVShader;
	ID3D11PixelShader*	 m_pPShader;
	ID3D11InputLayout*	 m_pInputLayout;
	ID3D11Buffer*		 m_pConstantBuffer;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;
};

