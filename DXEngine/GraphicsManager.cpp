/*



*/
#include "GraphicsManager.h"




bool GraphicsManager::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	m_pDXManager = std::make_unique<DXManager>();
	if (!m_pDXManager)
	{
		return false;
	}
	auto result = m_pDXManager->Init(screenWidth, screenHeight, hwnd, g_FULL_SCREEN, g_SCREEN_DEPTH, g_SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise DirectX", L"Error", MB_OK);
		return false;
	}
	return true;
}


void GraphicsManager::Shutdown()
{
	if (m_pDXManager)
	{
		m_pDXManager->Shutdown();
	}
}


bool GraphicsManager::Update()
{
	if (!Render())
	{
		return false;
	}

	return true;
}


bool GraphicsManager::Render()
{
	// Clear the buffers to begin the scene.
	m_pDXManager->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);


	// Present the rendered scene to the screen.
	m_pDXManager->EndScene();
	return true;
}
