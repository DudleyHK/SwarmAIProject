/*



*/
#include "GraphicsManager.h"
#include "Utilities.h"




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


	m_pCamera = std::make_unique<Camera>();
	if(!m_pCamera)
	{
		return false;
	}
	m_pCamera->SetPosition({0.f, 0.f, -10.f});

	m_pModelManager = std::make_unique<ModelManager>();
	if(!m_pModelManager)
	{
		return false;
	}

	result = m_pModelManager->Init(m_pDXManager->GetDevice(), "../DXEngine/ResourceFiles/CubeData.txt");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialise the Model Manager", L"Error", MB_OK);
		return false;
	}



	m_pColShaderManager = std::make_unique<ColourShaderManager>();
	if(!m_pColShaderManager)
	{
		return false;
	}

	result = m_pColShaderManager->Init(m_pDXManager->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialise the Colour Shader Manager", L"Error", MB_OK);
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

	if(m_pColShaderManager)
	{
		m_pColShaderManager->Shutdown();
	}

	if(m_pModelManager)
	{
		m_pModelManager->Shutdown();
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
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projMat;
	DirectX::XMMATRIX worldMat;

	static float rotation = 0.f;

	rotation += (float)DirectX::XM_PI * 0.01f;
	if(rotation > 360.f)
	{
		rotation -= 360.f;
	}


	m_pDXManager->BeginScene();

	m_pCamera->Render();

	m_pCamera->GetViewMatrix(viewMat);
	m_pDXManager->GetWorldMatrix(worldMat);
	m_pDXManager->GetProjectionMatrix(projMat);

	// Rotate the cube
	worldMat = DirectX::XMMatrixRotationY(rotation);

	m_pModelManager->Render(m_pDXManager->GetDeviceContext());

	auto result = m_pColShaderManager->Render(m_pDXManager->GetDeviceContext(), 
											  m_pModelManager->GetIndexCount(), 
											  worldMat, viewMat, projMat);
	if(!result)
	{
		return false;
	}


	m_pDXManager->EndScene();
	return true;
}
