/*
*/
#include <memory>
#include <vector>

#include <DirectXColors.h>

#include "DXManager.h"
#include "Utilities.h"



const bool DXManager::Init(const int screenWidth,
						   const int screenHeight,
						   const HWND hwnd,
						   const bool fullscreen,
						   const float screenDepth,
						   const float screenNear)
{
	HRESULT result;


	IDXGIFactory* pFactory = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIOutput*  pAdapterOutput = nullptr;
	unsigned int numModes = 0;
	unsigned int  numerator = 0;
	unsigned int denominator = 0;
	unsigned int stringLength = 0;


	// Create and use a graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	if(FAILED(result))
	{
		return false;
	}

	if(FAILED(pFactory->EnumAdapters(0, &pAdapter)))
	{
		return false;
	}

	result = pAdapter->EnumOutputs(0, &pAdapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Handle the display modes for the target monitor and video card combination.
	DXGI_MODE_DESC* pDisplayModeList = new DXGI_MODE_DESC[numModes];
	if(!pDisplayModeList)
	{
		return false;
	}

	result = pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, pDisplayModeList);
	if(FAILED(result))
	{
		return false;
	}

	for(unsigned int i = 0; i < numModes; i++)
	{
		if(pDisplayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(pDisplayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = pDisplayModeList[i].RefreshRate.Numerator;
				denominator = pDisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC adapterDesc = {0};
	result = pAdapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	auto error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Safely release the graphics handles.
	SafeDeleteArray(pDisplayModeList);
	SafeRelease(pAdapterOutput);
	SafeRelease(pAdapter);
	SafeRelease(pFactory);

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//#ifdef _DEBUG
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);


	DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;


	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, featureLevels, 1,
										   D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the back buffer data.
	ID3D11Texture2D* pBackBufferPtr = nullptr;
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	result = m_pDevice->CreateRenderTargetView(pBackBufferPtr, NULL, &m_pRenderTargetView);
	if(FAILED(result))
	{
		return false;
	}
	SafeRelease(pBackBufferPtr);


	// Create the render target view with the back buffer pointer.
	D3D11_TEXTURE2D_DESC depthBufferDesc = {0};
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {0};
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);


	// Set up the depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if(FAILED(result))
	{
		return false;
	}
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_RASTERIZER_DESC rasterDesc;
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

	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);
	if(FAILED(result))
	{
		return false;
	}
	m_pDeviceContext->RSSetState(m_pRasterState);


	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_pDeviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	auto fieldOfView = (float)DirectX::XM_PI / 4.0f;
	auto screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = DirectX::XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}


void DXManager::Shutdown()
{
	if(m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}

	SafeRelease(m_pRasterState);
	SafeRelease(m_pDepthStencilView);
	SafeRelease(m_pDepthStencilState);
	SafeRelease(m_pDepthStencilBuffer);
	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pDeviceContext);
	SafeRelease(m_pDevice);
	SafeRelease(m_pSwapChain);
}

/*
Create colour to clear back buffer to.
Clear the back buffer and depth buffer.
*/
void DXManager::BeginScene()
{
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::SkyBlue);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/* Show back buffer to screen now rendering is complete. */
void DXManager::EndScene()
{
	m_pSwapChain->Present(1, 0);
}


ID3D11Device* DXManager::GetDevice()
{
	return m_pDevice;
}


ID3D11DeviceContext* DXManager::GetDeviceContext()
{
	return m_pDeviceContext;
}

void DXManager::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void DXManager::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}


void DXManager::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void DXManager::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}