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
	if(!m_pDXManager)
	{
		return false;
	}
	result = m_pDXManager->Init(screenWidth, screenHeight, hwnd, g_FULL_SCREEN, g_SCREEN_DEPTH, g_SCREEN_NEAR);
	if(!result)
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
//	m_pCamera->SetPosition({-100.f, 50.f, -50.f});
//	m_pCamera->SetPosition({0.f, 0.f, -10.f});
//	m_pCamera->SetPosition({0.f, 0.f, -1000.f});
	m_pCamera->SetPosition({0.f, 0.f, -500.f});


	// Model Manager Object
	m_pModelManager = std::make_unique<ModelManager>();
	if(!m_pModelManager)
	{
		return false;
	}

	result = m_pModelManager->Init(m_pDXManager->GetDevice(), "../DXEngine/ResourceFiles/TetrahedronData.txt");
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

	
	m_pSwarmManager = std::make_unique<SwarmManager>();
	if(!m_pSwarmManager) 
		return false;
	m_pSwarmManager->Init(m_pDXManager->GetDevice(), m_pDXManager->GetDeviceContext(), hwnd, m_pModelManager->GetInstanceCount());


    //m_pCollisionsManager = std::make_unique<CollisionsManager>();
    //if(!m_pCollisionsManager)
    //	return false;
    //m_pCollisionsManager->Init(m_pModelManager->GetInstanceCount());


	return true;
}


void Application::Shutdown()
{
	if(m_pDXManager)
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

	if(m_pSwarmManager)
	{
		m_pSwarmManager->Shutdown();
	}

}

bool Application::HandleInput(float frameTime)
{
	bool keyDown;

	// Set the frame time for calculating the updated position.
	m_pPosition->SetFrameTime(frameTime);



	// Handle the input.

	auto multiply = m_pInput->IsLShiftPressed();
	m_pPosition->SetSpeedMultiplier(multiply);

	keyDown = m_pInput->IsLeftPressed() ? true : m_pInput->IsAPressed();
	m_pPosition->TurnLeft(keyDown);

	keyDown = m_pInput->IsRightPressed() ? true : m_pInput->IsDPressed();
	m_pPosition->TurnRight(keyDown);

	keyDown = m_pInput->IsUpPressed() ? true : m_pInput->IsWPressed();
	m_pPosition->MoveForward(keyDown);

	keyDown = m_pInput->IsDownPressed() ? true : m_pInput->IsSPressed();
	m_pPosition->MoveBackward(keyDown);

	keyDown = m_pInput->IsEPressed();
	m_pPosition->MoveUpward(keyDown);

	keyDown = m_pInput->IsQPressed();
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
	if(m_pInput->IsEscapePressed())
	{
		return false;
	}

	auto result = HandleInput(0.1f);
	if(!result) return false;


	// Handle Collisions
	// Update Active Particle List
	//for(auto i = 0; i < m_pModelManager->GetInstanceCount(); i++)
	//{
	//	auto particle = m_pSwarmManager->GetParticleAt(i);
	//	m_pCollisionsManager->AddParticle(particle);
	//}
	//m_pCollisionsManager->Update();



	// Update the swarm and pass the world matrices back to the ModelManager
	m_pSwarmManager->Update(m_pDXManager->GetDeviceContext());


	for(auto i = 0; i < m_pModelManager->GetInstanceCount(); i++)
	{
		auto worldMat = m_pSwarmManager->GetWorldAt(i);
		m_pModelManager->SetWorldAt(i, worldMat);
	}
	return true;
}


bool Application::Render()
{
	DirectX::XMMATRIX viewMat;
	DirectX::XMMATRIX projMat;
	DirectX::XMMATRIX worldMat;

	m_pDXManager->BeginScene();

	m_pCamera->Render();

	m_pCamera->GetViewMatrix(viewMat);
	m_pDXManager->GetWorldMatrix(worldMat);
	m_pDXManager->GetProjectionMatrix(projMat);

	m_pModelManager->Render(m_pDXManager->GetDeviceContext());

	auto result = m_pColShaderManager->Render(m_pDXManager->GetDeviceContext(),
											  m_pModelManager->GetVertexCount(),
											  m_pModelManager->GetInstanceCount(),
											  worldMat, viewMat, projMat);
	if(!result)
	{
		return false;
	}

	m_pDXManager->EndScene();
	return true;
}