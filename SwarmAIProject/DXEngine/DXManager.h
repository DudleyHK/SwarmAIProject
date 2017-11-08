/*



*/
#pragma once
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>


class DXManager
{
public:
	DXManager() = default;
	~DXManager() = default;

	const bool Init(const int screenWidth, const int screenHeight, const HWND hwnd,
					const bool fullscreen, const float screenDepth, const float screenNear);
	void Shutdown();

	void BeginScene();
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(DirectX::XMMATRIX&);
	void GetWorldMatrix(DirectX::XMMATRIX&);
	void GetOrthoMatrix(DirectX::XMMATRIX&);

	void GetVideoCardInfo(char* cardName, int& memory);

private:
	int  m_videoCardMemory;
	char m_videoCardDescription[128];

	// DX Attributes
	IDXGISwapChain*         m_pSwapChain;
	ID3D11Device*           m_pDevice;
	ID3D11DeviceContext*    m_pDeviceContext;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D*        m_pDepthStencilBuffer;
	ID3D11DepthStencilState* m_pDepthStencilState;
	ID3D11DepthStencilView*  m_pDepthStencilView;
	ID3D11RasterizerState*   m_pRasterState;

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;
};

