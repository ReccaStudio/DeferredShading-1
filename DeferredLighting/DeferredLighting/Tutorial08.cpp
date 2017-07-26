//--------------------------------------------------------------------------------------
// File: Tutorial08.cpp
//
// Basic introduction to DXUT
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "SDKmisc.h"

#pragma warning( disable : 4100 )

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

struct CBChangesEveryFrame
{
    XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};

struct LightBufferType
{
	XMFLOAT3 lightDirection;
	float padding;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
const int					BUFFER_COUNT = 2;
const int					screen_width = 800;
const int					screen_height = 600;
ID3D11VertexShader*         g_pVertexShader = nullptr;
ID3D11VertexShader*         g_pLightVertexShader = nullptr;
ID3D11PixelShader*          g_pPixelShader = nullptr;
ID3D11PixelShader*          g_pLightPixelShader = nullptr;
ID3D11InputLayout*          g_pVertexLayout = nullptr;
ID3D11InputLayout*          g_pLightVertexLayout = nullptr;
ID3D11Buffer*               g_pVertexBuffer = nullptr;
ID3D11Buffer*               g_pQuadVertexBuffer = nullptr;
ID3D11Buffer*               g_pLightPixelBuffer = nullptr;
ID3D11Buffer*               g_pIndexBuffer = nullptr;
ID3D11Buffer*               g_pQuadIndexBuffer = nullptr;
ID3D11Buffer*               g_pCBChangesEveryFrame = nullptr;
ID3D11ShaderResourceView*   g_pTextureRV = nullptr;
ID3D11SamplerState*         g_pSamplerLinear = nullptr;
ID3D11SamplerState*         g_pSamplerLight = nullptr;
XMMATRIX                    g_World;
XMMATRIX                    g_View;
XMMATRIX                    g_Projection;
XMFLOAT4                    g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
ID3D11Texture2D*			g_renderTargetTextureArray[BUFFER_COUNT];
ID3D11RenderTargetView*		g_renderTargetViewArray[BUFFER_COUNT];
ID3D11RenderTargetView*		g_renderTargetView = nullptr;
ID3D11ShaderResourceView*	g_shaderResourceViewArray[BUFFER_COUNT];
ID3D11Texture2D*			g_depthStencilBuffer = nullptr;
ID3D11DepthStencilView*		g_depthStencilView = nullptr;
ID3D11DepthStencilState*	g_depthStencilState = nullptr;
ID3D11DepthStencilState*	g_depthDisabledStencilState = nullptr;
ID3D11RasterizerState*		g_rasterState = nullptr;
ID3D11RasterizerState*		g_rasterStateNoCulling = nullptr;
ID3D11BlendState*			g_alphaEnableBlendingState = nullptr;
ID3D11BlendState*			g_alphaDisableBlendingState = nullptr;
D3D11_VIEWPORT				g_viewport;

SimpleVertex vertices[] =
{
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f,1.0f,0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f,1.0f,0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f,1.0f,0.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f,1.0f,0.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f,-1.0f,0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f,-1.0f,0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f,-1.0f,0.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f,-1.0f,0.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(-1.0f,0.0f,0.0f) },
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(-1.0f,0.0f,0.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(-1.0f,0.0f,0.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(-1.0f,0.0f,0.0f) },

	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(1.0f,0.0f,0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(1.0f,0.0f,0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(1.0f,0.0f,0.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(1.0f,0.0f,0.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f,0.0f,-1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f,0.0f,-1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f,0.0f,-1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f,0.0f,-1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f),XMFLOAT3(0.0f,0.0f,1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f),XMFLOAT3(0.0f,0.0f,1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(0.0f,0.0f,1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f),XMFLOAT3(0.0f,0.0f,1.0f) },
};

DWORD indices[] =
{
	3,1,0,
	2,1,3,

	6,4,5,
	7,4,6,

	11,9,8,
	10,9,11,

	14,12,13,
	15,12,14,

	19,17,16,
	18,17,19,

	22,20,21,
	23,20,22
};

void SetRenderTargets(ID3D11DeviceContext* deviceContext);
void SetRenderTargets(ID3D11DeviceContext* deviceContext);
void TurnZBufferOn(ID3D11DeviceContext* deviceContext);
void TurnZBufferOff(ID3D11DeviceContext* deviceContext);
void SetBackBufferRenderTarget(ID3D11DeviceContext* deviceContext);
void ResetViewport(ID3D11DeviceContext* deviceContext);

bool InitializeQuadBuffers(ID3D11Device* device, int windowWidth, int windowHeight)
{
	float left, right, top, bottom;
	SimpleVertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, m_vertexCount, m_indexCount;


	// Calculate the screen coordinates of the left side of the window.
	left = (float)((windowWidth / 2) * -1);

	// Calculate the screen coordinates of the right side of the window.
	right = left + (float)windowWidth;

	// Calculate the screen coordinates of the top of the window.
	top = (float)(windowHeight / 2);

	// Calculate the screen coordinates of the bottom of the window.
	bottom = top - (float)windowHeight;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new SimpleVertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].Pos = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].Tex = XMFLOAT2(0.0f, 0.0f);

	vertices[1].Pos = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].Tex = XMFLOAT2(1.0f, 1.0f);

	vertices[2].Pos = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].Tex = XMFLOAT2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].Pos = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].Tex = XMFLOAT2(0.0f, 0.0f);

	vertices[4].Pos = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].Tex = XMFLOAT2(1.0f, 0.0f);

	vertices[5].Pos = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].Tex = XMFLOAT2(1.0f, 1.0f);

	// Load the index array with data.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &g_pQuadVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &g_pQuadIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void RenderBuffers(ID3D11DeviceContext* deviceContext, ID3D11Buffer* m_vertexBuffer,ID3D11Buffer* m_indexBuffer)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SimpleVertex);
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
    HRESULT hr = S_OK;

    auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    V_RETURN( DXUTCompileFromFile( L"Deferred.hlsl", nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pVSBlob ) );

    // Create the vertex shader
    hr = pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        SAFE_RELEASE( pVSBlob );
        return hr;
    }

    // Define the input layout
     D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &g_pVertexLayout );
    SAFE_RELEASE( pVSBlob );
    if( FAILED( hr ) )
        return hr;

    // Set the input layout


    // Compile the pixel shader
    ID3DBlob* pPSBlob  = nullptr;
    V_RETURN( DXUTCompileFromFile( L"Deferred.hlsl", nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPSBlob  ) );

    // Create the pixel shader
    hr = pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader );
    SAFE_RELEASE( pPSBlob );
    if( FAILED( hr ) )
        return hr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    V_RETURN( pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer ) );

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( DWORD ) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    V_RETURN( pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer ) );

    // Create the constant buffers
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    V_RETURN( pd3dDevice->CreateBuffer( &bd, nullptr, &g_pCBChangesEveryFrame ) );

	RenderBuffers(pd3dImmediateContext, g_pVertexBuffer, g_pIndexBuffer);

    // Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	static const XMVECTORF32 s_Eye = { 0.0f, 3.0f, -6.0f, 0.f };
	static const XMVECTORF32 s_At = { 0.0f, 1.0f, 0.0f, 0.f };
	static const XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.f };
	g_View = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

	// Load the Texture
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"misc\\seafloor.dds", &g_pTextureRV));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear));


	InitializeQuadBuffers(pd3dDevice, screen_width, screen_height);

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc,depthDisabledStencilDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_BLEND_DESC blendStateDescription;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screen_width;
	depthBufferDesc.Height = screen_height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	V_RETURN(pd3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &g_depthStencilBuffer));

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	V_RETURN(pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &g_depthStencilState));

	// Set the depth stencil state.
	pd3dImmediateContext->OMSetDepthStencilState(g_depthStencilState, 1);

	//depthDisabled
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	V_RETURN(pd3dDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &g_depthDisabledStencilState));

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	V_RETURN(pd3dDevice->CreateDepthStencilView(g_depthStencilBuffer, &depthStencilViewDesc, &g_depthStencilView));
	
	auto renderTargetView = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->OMSetRenderTargets(1, &renderTargetView, g_depthStencilView);
	
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	V_RETURN(pd3dDevice->CreateRasterizerState(&rasterDesc, &g_rasterState));
	pd3dImmediateContext->RSSetState(g_rasterState);

	// Setup a raster description which turns off back face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	V_RETURN(pd3dDevice->CreateRasterizerState(&rasterDesc, &g_rasterStateNoCulling));

	g_viewport.Width = screen_width;
	g_viewport.Height = screen_height;
	g_viewport.MinDepth = 0.0f;
	g_viewport.MaxDepth = 1.0f;
	g_viewport.TopLeftX = 0.0f;
	g_viewport.TopLeftY = 0.0f;
	pd3dImmediateContext->RSSetViewports(1, &g_viewport);

	//blendState
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	blendStateDescription.AlphaToCoverageEnable = TRUE;
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	V_RETURN(pd3dDevice->CreateBlendState(&blendStateDescription, &g_alphaEnableBlendingState));

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the second blend state using the description.
	V_RETURN(pd3dDevice->CreateBlendState(&blendStateDescription, &g_alphaDisableBlendingState));
	

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	// Setup the render target texture description.
	textureDesc.Width = screen_width;
	textureDesc.Height = screen_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		V_RETURN(pd3dDevice->CreateTexture2D(&textureDesc, NULL, &g_renderTargetTextureArray[i]));
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		V_RETURN(pd3dDevice->CreateRenderTargetView(g_renderTargetTextureArray[i], &renderTargetViewDesc, &g_renderTargetViewArray[i]));
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		V_RETURN(pd3dDevice->CreateShaderResourceView(g_renderTargetTextureArray[i], &shaderResourceViewDesc, &g_shaderResourceViewArray[i]));
	}

	//pd3dImmediateContext->OMSetRenderTargets(BUFFER_COUNT, g_renderTargetViewArray, g_depthStencilView);
	//pd3dImmediateContext->RSSetViewports(1, &g_viewport);

	// light
	// Create the pixel shader

	// Compile the vertex shader
	ID3DBlob* pLightVSBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Light.hlsl", nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pLightVSBlob));
	
	// Create the vertex shader
	hr = pd3dDevice->CreateVertexShader(pLightVSBlob->GetBufferPointer(), pLightVSBlob->GetBufferSize(), nullptr, &g_pLightVertexShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pLightVSBlob);
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC LightLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = ARRAYSIZE(LightLayout);

	// Create the input layout
	hr = pd3dDevice->CreateInputLayout(LightLayout, numElements, pLightVSBlob->GetBufferPointer(),
		pLightVSBlob->GetBufferSize(), &g_pLightVertexLayout);
	SAFE_RELEASE(pVSBlob);
	if (FAILED(hr))
		return hr;

	//// Set the input layout
	//pd3dImmediateContext->IASetInputLayout(g_pLightVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSLightBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Light.hlsl", nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPSLightBlob));


	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.BorderColor[0] = 0;
	sampDesc.BorderColor[1] = 0;
	sampDesc.BorderColor[2] = 0;
	sampDesc.BorderColor[3] = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLight));

	D3D11_BUFFER_DESC lightbd;
	ZeroMemory(&lightbd, sizeof(lightbd));
	lightbd.Usage = D3D11_USAGE_DYNAMIC;
	lightbd.ByteWidth = sizeof(LightBufferType);
	lightbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = vertices;
	V_RETURN(pd3dDevice->CreateBuffer(&lightbd, NULL, &g_pLightPixelBuffer));

//	SetRenderTargets(pd3dImmediateContext);
//	RenderBuffers(pd3dImmediateContext,g_pVertexBuffer,g_pIndexBuffer);
//
    return S_OK;
}

void SetRenderTargets(ID3D11DeviceContext* deviceContext)
{
	// Bind the render target view array and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, g_renderTargetViewArray, g_depthStencilView);

	// Set the viewport.
	deviceContext->RSSetViewports(1, &g_viewport);

	return;
}

void ClearRenderTargets(ID3D11DeviceContext* deviceContext)
{
	for (int i = 0; i<BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(g_renderTargetViewArray[i], Colors::MidnightBlue);
	}

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void TurnZBufferOff(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(g_depthDisabledStencilState, 1);
	return;
}

void TurnZBufferOn(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetDepthStencilState(g_depthStencilState, 1);
	return;
}

void SetBackBufferRenderTarget(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);
}

void ResetViewport(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetViewports(1, &g_viewport);
}
//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    // Setup the projection parameters
    float fAspect = static_cast<float>( pBackBufferSurfaceDesc->Width ) / static_cast<float>( pBackBufferSurfaceDesc->Height );
    g_Projection = XMMatrixPerspectiveFovLH( XM_PI * 0.25f, fAspect, 0.1f, 100.0f );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Rotate cube around the origin
    g_World = XMMatrixRotationY( 60.0f * XMConvertToRadians((float)fTime) );

    // Modify the color
    //g_vMeshColor.x = ( sinf( ( float )fTime * 1.0f ) + 1.0f ) * 0.5f;
    //g_vMeshColor.y = ( cosf( ( float )fTime * 3.0f ) + 1.0f ) * 0.5f;
    //g_vMeshColor.z = ( sinf( ( float )fTime * 5.0f ) + 1.0f ) * 0.5f;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
	//g_renderTargetView = DXUTGetD3D11RenderTargetView();
	//SetRenderTargets(pd3dImmediateContext);
	//ClearRenderTargets(pd3dImmediateContext);

	//RenderBuffers(pd3dImmediateContext,g_pVertexBuffer,g_pIndexBuffer);

	// Update constant buffer that changes once per frame
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);

	//
	// Clear the depth stencil
	//
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	V(pd3dImmediateContext->Map(g_pCBChangesEveryFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	auto pCB = reinterpret_cast<CBChangesEveryFrame*>(MappedResource.pData);
	XMStoreFloat4x4(&pCB->mWorld, XMMatrixTranspose(g_World));
	XMStoreFloat4x4(&pCB->mView, XMMatrixTranspose(g_View));
	XMStoreFloat4x4(&pCB->mProj, XMMatrixTranspose(g_Projection));
	pd3dImmediateContext->Unmap(g_pCBChangesEveryFrame, 0);

	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBChangesEveryFrame);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pCBChangesEveryFrame);
	pd3dImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);

	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout);
	pd3dImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pd3dImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

	pd3dImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
	pd3dImmediateContext->DrawIndexed(36, 0, 0);

	//SetBackBufferRenderTarget(pd3dImmediateContext);
	//ResetViewport(pd3dImmediateContext);

	//pd3dImmediateContext->ClearRenderTargetView(g_renderTargetView, Colors::MidnightBlue);
	//pd3dImmediateContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	////
	//// Render Full Screen Quad
	////
	//TurnZBufferOff(pd3dImmediateContext);

	//RenderBuffers(pd3dImmediateContext,g_pQuadVertexBuffer,g_pQuadIndexBuffer);
	////
	//// Light
	////
	//V(pd3dImmediateContext->Map(g_pCBChangesEveryFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	//pCB = reinterpret_cast<CBChangesEveryFrame*>(MappedResource.pData);
	//XMStoreFloat4x4(&pCB->mWorld, XMMatrixTranspose(g_World));
	//XMStoreFloat4x4(&pCB->mView, XMMatrixTranspose(g_View));
	//XMStoreFloat4x4(&pCB->mProj, XMMatrixTranspose(g_Projection));
	//pd3dImmediateContext->Unmap(g_pCBChangesEveryFrame, 0);
	//
	//pd3dImmediateContext->VSSetShader(g_pLightVertexShader, nullptr, 0);
	//pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBChangesEveryFrame);

	//V(pd3dImmediateContext->Map(g_pLightPixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	//auto pData = reinterpret_cast<LightBufferType*>(MappedResource.pData);
	//pData->lightDirection= XMFLOAT3(0.0f, 0.0f, 1.0f);
	//pData->padding = 0.0f;
	//pd3dImmediateContext->Unmap(g_pLightPixelBuffer, 0);

	//pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pLightPixelBuffer);
	//pd3dImmediateContext->PSSetShaderResources(0, 1, &g_shaderResourceViewArray[0]);//color
	//pd3dImmediateContext->PSSetShaderResources(1, 1, &g_shaderResourceViewArray[1]);//normal

	//pd3dImmediateContext->IASetInputLayout(g_pLightVertexLayout);
	//pd3dImmediateContext->VSSetShader(g_pLightVertexShader, nullptr, 0);
	//pd3dImmediateContext->PSSetShader(g_pLightPixelShader, nullptr, 0);
	//pd3dImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLight);
	//pd3dImmediateContext->DrawIndexed(6, 0, 0);

	//TurnZBufferOn(pd3dImmediateContext);

	//SetBackBufferRenderTarget(pd3dImmediateContext);
	//ResetViewport(pd3dImmediateContext);

}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    SAFE_RELEASE( g_pVertexBuffer );
    SAFE_RELEASE( g_pIndexBuffer );
    SAFE_RELEASE( g_pVertexLayout );
    SAFE_RELEASE( g_pTextureRV );
    SAFE_RELEASE( g_pVertexShader );
    SAFE_RELEASE( g_pPixelShader );
    SAFE_RELEASE( g_pCBChangesEveryFrame );
    SAFE_RELEASE( g_pSamplerLinear );
	SAFE_RELEASE( g_depthStencilBuffer );
	SAFE_RELEASE( g_depthStencilView );
	SAFE_RELEASE(g_pLightVertexShader);
	SAFE_RELEASE(g_pLightPixelShader);
	SAFE_RELEASE(g_pLightVertexLayout); 
	SAFE_RELEASE(g_pQuadVertexBuffer);
	SAFE_RELEASE(g_pLightPixelBuffer) ;
	SAFE_RELEASE(g_pQuadIndexBuffer);
	SAFE_RELEASE(g_pSamplerLinear);
	SAFE_RELEASE(g_pSamplerLight);
	SAFE_RELEASE(g_renderTargetView);
	SAFE_RELEASE(g_depthStencilBuffer);
	SAFE_RELEASE(g_depthStencilView);
	SAFE_RELEASE(g_depthStencilState);
	SAFE_RELEASE(g_depthDisabledStencilState);
	SAFE_RELEASE(g_rasterState);
	SAFE_RELEASE(g_rasterStateNoCulling);
	SAFE_RELEASE(g_alphaEnableBlendingState);
	SAFE_RELEASE(g_alphaDisableBlendingState);
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		SAFE_RELEASE(g_renderTargetViewArray[i]);
		SAFE_RELEASE(g_renderTargetTextureArray[i]);
		SAFE_RELEASE(g_shaderResourceViewArray[i]);
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: // Change as needed                
                break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

    // Perform any application-level initialization here

    DXUTInit( true, true, nullptr ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Tutorial08" );

    // Only require 10-level hardware or later
    DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0, true, screen_width, screen_height );
    DXUTMainLoop(); // Enter into the DXUT render loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}
