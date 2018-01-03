#include "SkyBox.h"



SkyBox::SkyBox(ID3D11Device* D3D11Device, ID3D11DeviceContext* ImmediateContext) : Mesh(D3D11Device, ImmediateContext)
{
	
}


SkyBox::~SkyBox()
{
	if (m_pRasterSolid != 0) m_pRasterSolid->Release();
	if (m_pRasterSkyBox != 0) m_pRasterSkyBox->Release();
	if (m_pDepthWriteSolid != 0) m_pDepthWriteSolid->Release();
	if (m_pDepthWriteSkyBox != 0) m_pDepthWriteSkyBox->Release();
}

int SkyBox::LoadObjModel(char * fileName)
{
	m_pObject = new ObjFileModel(fileName, m_pD3D11Device, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	HRESULT hr = S_OK;
	// Create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	// Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 64;  // Must Be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Use as a constant buffer

	hr = m_pD3D11Device->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	//create a rasterizer derscription so i can change the cull mode
	//for the sky box to be visable
	D3D11_RASTERIZER_DESC rasterizer_desc;
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;

	hr = m_pD3D11Device->CreateRasterizerState(&rasterizer_desc, &m_pRasterSolid);

	if (FAILED(hr))
	{
		return hr;
	}

	rasterizer_desc.CullMode = D3D11_CULL_FRONT;
	hr = m_pD3D11Device->CreateRasterizerState(&rasterizer_desc, &m_pRasterSkyBox);

	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	hr = m_pD3D11Device->CreateDepthStencilState(&depth_stencil_desc, &m_pDepthWriteSolid);

	if (FAILED(hr))
	{
		return hr;
	}

	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_pD3D11Device->CreateDepthStencilState(&depth_stencil_desc, &m_pDepthWriteSkyBox);

	if (FAILED(hr))
	{
		return hr;
	}

	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))exit(0);
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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

void SkyBox::Draw(XMMATRIX * view, XMMATRIX * projection)
{
	//m_directional_light_shines_from = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	//m_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

	//m_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	XMMATRIX world , transpose;
	MODEL_CONSTANT_BUFFER model_cb_values;

	world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	world *= XMMatrixScaling(m_scale, m_scale, m_scale);

	world *= XMMatrixTranslation(m_x, m_y, m_z);

	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	transpose = XMMatrixTranspose(world); // model world matrix

	// upload the new values for the constant buffer
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);


	m_pImmediateContext->RSSetState(m_pRasterSkyBox);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSkyBox, 1);

	m_pObject->Draw();

	m_pImmediateContext->RSSetState(m_pRasterSolid);
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSolid, 1);

	
}

void SkyBox::AddTexture(char * fileName)
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