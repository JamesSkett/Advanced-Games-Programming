#pragma once
#include "Mesh.h"
class SkyBox :
	public Mesh
{
public:
	SkyBox(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext);
	~SkyBox();

	int LoadObjModel(char* fileName);
	void Draw(XMMATRIX* view, XMMATRIX* projection);
	void AddTexture(char * fileName);

private:
	ID3D11ShaderResourceView* m_pTextureCube;

	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer0;

};

struct POS_COL_TEX_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
};