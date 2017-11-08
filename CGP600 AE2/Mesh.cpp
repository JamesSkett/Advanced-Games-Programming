#include "Mesh.h"



Mesh::Mesh(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3D11Device = D3D11Device;
	m_pImmediateContext = ImmediateContext;

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;
	m_scale = 1.0f;
}

Mesh::~Mesh()
{
	delete m_pObject;
	m_pObject = nullptr;

	if (m_pInputLayout)  m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader)  m_pPShader->Release();
	if (m_pTexture)     m_pTexture->Release();
	//if (m_pSampler0)     m_pSampler0->Release();

	if (m_pConstantBuffer)  m_pConstantBuffer->Release();
	//if (m_pImmediateContext) m_pImmediateContext->Release();
	//if (m_pD3D11Device)        m_pD3D11Device->Release();
}

int Mesh::LoadObjModel(char* fileName)
{
	
	m_pObject = new ObjFileModel(fileName, m_pD3D11Device, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	HRESULT hr = S_OK;
	// Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	// Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 112;  // Must Be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Use as a constant buffer

	hr = m_pD3D11Device->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))exit(0);
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) exit(0);
	}

	// Create shader objects
	hr = m_pD3D11Device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr)) exit(0);

	hr = m_pD3D11Device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr)) exit(0);

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,    0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	hr = m_pD3D11Device->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr)) exit(0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);




	return 0;
}

void Mesh::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	m_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

	m_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	m_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	XMMATRIX meshWorld;

	meshWorld = XMMatrixRotationX(XMConvertToRadians(m_xangle)) * XMMatrixRotationY(XMConvertToRadians(m_yangle)) * XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	meshWorld *= XMMatrixScaling(m_scale, m_scale, m_scale);
	meshWorld *= XMMatrixTranslation(m_x, m_y, m_z);

	XMMATRIX transpose;
	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = meshWorld * (*view) * (*projection);
	//model_cb_values.World = XMMatrixTranspose(meshWorld);


	transpose = XMMatrixTranspose(meshWorld); // model world matrix


	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from, transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	// upload the new values for the constant buffer
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pObject->Draw();

}

void Mesh::AddTexture(char * fileName)
{
	D3DX11CreateShaderResourceViewFromFile(m_pD3D11Device, fileName, NULL, NULL, &m_pTexture, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3D11Device->CreateSamplerState(&sampler_desc, &m_pSampler0);
}

void Mesh::SetXPos(float x_pos)
{
	m_x = x_pos;
}

void Mesh::SetYPos(float y_pos)
{
	m_y = y_pos;
}

void Mesh::SetZPos(float z_pos)
{
	m_z = z_pos;
}

void Mesh::SetXAngle(float x_angle)
{
	m_xangle = x_angle;
}

void Mesh::SetYAngle(float y_angle)
{
	m_yangle = y_angle;
}

void Mesh::SetZAngle(float z_angle)
{
	m_zangle = z_angle;
}

void Mesh::SetScale(float scale)
{
	m_scale = scale;
}

float Mesh::GetXPos()
{
	return m_x;
}

float Mesh::GetYPos()
{
	return m_y;
}

float Mesh::GetZPos()
{
	return m_z;
}

float Mesh::GetXAngle()
{
	return m_xangle;
}

float Mesh::GetYAndle()
{
	return m_yangle;
}

float Mesh::GetZAngle()
{
	return m_zangle;
}

float Mesh::GetScale()
{
	return m_scale;
}

void Mesh::UpdateXPos(float distance)
{
	m_x += distance;
}

void Mesh::UpdateYPos(float distance)
{
	m_y += distance;
}

void Mesh::UpdateZPos(float distance)
{
	m_z += distance;
}

void Mesh::UpdateXAngle(float angle)
{
	m_xangle += angle;
}

void Mesh::UpdateYAngle(float angle)
{
	m_yangle += angle;
}

void Mesh::UpdateZAngle(float angle)
{
	m_zangle += angle;
}

void Mesh::UpdateScale(float scale)
{
	m_scale += scale;
}

void Mesh::Lookat_XZ(float targetX, float targetY, float targetZ)
{
	m_yangle = atan2((targetX - m_x), (targetZ - m_z)) * (180 / XM_PI);

	float xzDiff = sqrt(pow(m_x, 2) + pow(targetZ, 2));

	m_xangle = -atan2((targetY - m_y), xzDiff) * (180 / XM_PI);
}



