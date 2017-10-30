#include "Mesh.h"



Mesh::Mesh(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3D11Device = D3D11Device;
	m_pImmediateContext = ImmediateContext;
}

Mesh::~Mesh()
{

}

int Mesh::LoadObjModel(char* fileName)
{
	
	m_pObject = new ObjFileModel(fileName, m_pD3D11Device, m_pImmediateContext);


	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	/*hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))exit(0);
	}

	hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) exit(0);
	}*/
	return 0;
}
