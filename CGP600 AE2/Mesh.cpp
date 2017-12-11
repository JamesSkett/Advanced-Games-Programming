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

	m_dx = sin(XMConvertToRadians(m_xangle));
	m_dz = cos(XMConvertToRadians(m_zangle));
	m_dy = tan(XMConvertToRadians(m_yangle));

	xyz xyz1 = { 1, 2, 3 };
	xyz xyz2 = { 3, 4, 1 };
	xyz xyz3 = { 4, 5, 6 };

	Plane dot1 = math.PlaneVal(&xyz1, &xyz2, &xyz3);
	float planeVal = math.CalculatePlaneValForPoint(&dot1, &xyz1);
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


	//CalculateModelCentrePoint();
	//CalculateBoundingSphereRadius();

	return 0;
}

void Mesh::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	m_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

	m_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	m_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

	XMMATRIX transpose;
	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = (*world) * (*view) * (*projection);


	transpose = XMMatrixTranspose((*world)); // model world matrix


	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.directional_light_vector = DirectX::XMVector3Transform(m_directional_light_shines_from, transpose);
	model_cb_values.directional_light_vector = DirectX::XMVector3Normalize(model_cb_values.directional_light_vector);

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

float Mesh::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius * m_scale;
}

float Mesh::GetBoundingSphere_x()
{
	return m_bounding_sphere_centre_x;
}

float Mesh::GetBoundingSphere_y()
{
	return m_bounding_sphere_centre_y;
}

float Mesh::GetBoundingSphere_z()
{
	return m_bounding_sphere_centre_z;
}

ObjFileModel* Mesh::GetObject()
{
	return m_pObject;
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

	m_dx = sin(XMConvertToRadians(m_yangle));
	//m_dy = atan(XMConvertToRadians(m_xangle));
	m_dz = cos(XMConvertToRadians(m_yangle));

}

void Mesh::UpdateYAngle(float angle)
{
	m_yangle += angle;

	m_dx = sin(XMConvertToRadians(m_yangle));
	//m_dy = atan(XMConvertToRadians(m_xangle));
	m_dz = cos(XMConvertToRadians(m_yangle));
}

void Mesh::UpdateZAngle(float angle)
{
	m_zangle += angle;

	//m_dz = tan(XMConvertToRadians(m_zangle));
}

void Mesh::UpdateScale(float scale)
{
	m_scale += scale;
}

void Mesh::MoveForward(float speed)
{
	m_x += m_dx * speed;
	//m_y += m_dy * speed;
	m_z += m_dz * speed;
}

void Mesh::Lookat_XZ(float targetX, float targetY, float targetZ)
{
	m_yangle = atan2((targetX - m_x), (targetZ - m_z)) * (180 / XM_PI);

	float xzDiff = sqrt(pow(m_x, 2) + pow(targetZ, 2));

	m_xangle = -atan2((targetY - m_y), xzDiff) * (180 / XM_PI);
}

XMVECTOR Mesh::GetBoundingSphereWorldSpacePosition()
{
	XMMATRIX world;

	world = DirectX::XMMatrixRotationX(XMConvertToRadians(m_xangle)) * DirectX::XMMatrixRotationY(XMConvertToRadians(m_yangle)) * DirectX::XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	world *= DirectX::XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= DirectX::XMMatrixTranslation(m_x + m_dx, m_y + m_dy, m_z + m_dy);

	XMVECTOR offset;

	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0f);

	offset = XMVector3Transform(offset, world);

	return offset;
}

bool Mesh::CheckCollision(Mesh * targetMesh)
{
	if (this == targetMesh)
		return false;

	XMVECTOR sphere1CentrePos = GetBoundingSphereWorldSpacePosition();
	XMVECTOR sphere2CentrePos = targetMesh->GetBoundingSphereWorldSpacePosition();

	float x1 = XMVectorGetX(sphere1CentrePos);
	float x2 = XMVectorGetX(sphere2CentrePos);
	float y1 = XMVectorGetY(sphere1CentrePos);
	float y2 = XMVectorGetY(sphere2CentrePos);
	float z1 = XMVectorGetZ(sphere1CentrePos);
	float z2 = XMVectorGetZ(sphere2CentrePos);

	float distBetweenSpheres = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

	float model1Radius = GetBoundingSphereRadius();
	float model2Radius = targetMesh->GetBoundingSphereRadius();

	float sumOfRadii = model1Radius + model2Radius;

	if (distBetweenSpheres <= sumOfRadii)
	{
		return true;
	}
	else
		return false;
}

void Mesh::CalculateModelCentrePoint()
{
	float maxPosX = m_pObject->vertices[0].Pos.x;
	float minPosX = m_pObject->vertices[0].Pos.x;

	float maxPosY = m_pObject->vertices[0].Pos.y;
	float minPosY = m_pObject->vertices[0].Pos.y;

	float maxPosZ = m_pObject->vertices[0].Pos.z;
	float minPosZ = m_pObject->vertices[0].Pos.z;


	for (unsigned int i = 1; i < m_pObject->numverts; i++)
	{
		if (m_pObject->vertices[i].Pos.x > maxPosX)
		{
			maxPosX = m_pObject->vertices[i].Pos.x;
		}

		if (m_pObject->vertices[i].Pos.x < minPosX)
		{
			minPosX = m_pObject->vertices[i].Pos.x;
		}

		if (m_pObject->vertices[i].Pos.y > maxPosY)
		{
			maxPosY = m_pObject->vertices[i].Pos.y;
		}

		if (m_pObject->vertices[i].Pos.y < minPosY)
		{
			minPosY = m_pObject->vertices[i].Pos.y;
		}

		if (m_pObject->vertices[i].Pos.z > maxPosZ)
		{
			maxPosZ = m_pObject->vertices[i].Pos.z;
		}

		if (m_pObject->vertices[i].Pos.z < minPosZ)
		{
			minPosZ = m_pObject->vertices[i].Pos.z;
		}
	}

	m_bounding_sphere_centre_x = (maxPosX + minPosX) / 2;
	m_bounding_sphere_centre_y = (maxPosY + minPosY) / 2;
	m_bounding_sphere_centre_z = (maxPosZ + minPosZ) / 2;

}

void Mesh::CalculateBoundingSphereRadius()
{
	float maxDistance = sqrt((pow(m_pObject->vertices[0].Pos.x - m_bounding_sphere_centre_x, 2)) +
							 (pow(m_pObject->vertices[0].Pos.y - m_bounding_sphere_centre_y, 2)) +
							 (pow(m_pObject->vertices[0].Pos.z - m_bounding_sphere_centre_z, 2)));

	float currentDistance;

	for (unsigned int i = 1; i < m_pObject->numverts; i++)
	{
		currentDistance = sqrt((pow(m_pObject->vertices[i].Pos.x - m_bounding_sphere_centre_x, 2)) +
							   (pow(m_pObject->vertices[i].Pos.y - m_bounding_sphere_centre_y, 2)) +
							   (pow(m_pObject->vertices[i].Pos.z - m_bounding_sphere_centre_z, 2)));

		if (currentDistance > maxDistance)
		{
			maxDistance = currentDistance;
		}
	}

	m_bounding_sphere_radius = maxDistance;

}



