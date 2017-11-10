/*



*/
#include "Application.h"
#include "Utilities.h"




bool Application::Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	// Input Object
	m_pInput = std::make_unique<InputManager>();
	if(!m_pInput)
	{
		return false;
	}

	auto result = m_pInput->Init(hInstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialise the Input object.", L"Error", MB_OK);
		return false;
	}

	// DX Object
	m_pDXManager = std::make_unique<DXManager>();
	if (!m_pDXManager)
	{
		return false;
	}
	result = m_pDXManager->Init(screenWidth, screenHeight, hwnd, g_FULL_SCREEN, g_SCREEN_DEPTH, g_SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise DirectX", L"Error", MB_OK);
		return false;
	}


	// Camera Object
	m_pCamera = std::make_unique<Camera>();
	if(!m_pCamera)
	{
		return false;
	}
	m_pCamera->SetPosition({0.f, 0.f, -10.f});



	// Model Manager Object
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

	// Colour Shader Object
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


	// Timer Object


	// Position Object
	m_pPosition = std::make_unique<Position>();
	if(!m_pPosition)
	{
		return false;
	}
	m_pPosition->SetPosition(m_pCamera->GetPosition());




	return true;
}


void Application::Shutdown()
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

	if(m_pInput)
	{
		m_pInput->Shutdown();
	}

}

bool Application::HandleInput(float frameTime)
{
	bool keyDown;

	// Set the frame time for calculating the updated position.
	m_pPosition->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_pInput->IsLeftPressed();
	m_pPosition->TurnLeft(keyDown);

	keyDown = m_pInput->IsRightPressed();
	m_pPosition->TurnRight(keyDown);

	keyDown = m_pInput->IsUpPressed();
	m_pPosition->MoveForward(keyDown);

	keyDown = m_pInput->IsDownPressed();
	m_pPosition->MoveBackward(keyDown);

	keyDown = m_pInput->IsAPressed();
	m_pPosition->MoveUpward(keyDown);

	keyDown = m_pInput->IsZPressed();
	m_pPosition->MoveDownward(keyDown);

	keyDown = m_pInput->IsPgUpPressed();
	m_pPosition->LookUpward(keyDown);

	keyDown = m_pInput->IsPgDownPressed();
	m_pPosition->LookDownward(keyDown);

	// Get the view point position/rotation.
	auto position = m_pPosition->GetPosition();
	auto rotation = m_pPosition->GetRotation();

	// Set the position of the camera.
	m_pCamera->SetPosition(position);
	m_pCamera->SetRotation(rotation);

	return true;

}


bool Application::Update()
{
	if(!m_pInput->Update())
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if(m_pInput->IsEscapePressed() == true)
	{
		return false;
	}

	auto result = HandleInput(0.01f);
	if(!result) return false;

	return true;
}


bool Application::Render()
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