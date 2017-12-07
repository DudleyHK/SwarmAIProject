/*
*/
#include <iostream>


#include "Application.h"
#include "Utilities.h"




bool Application::Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_hwnd = hwnd;

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


	//////////////// MOUSE TO WORLD //////////////////////////
	DirectX::XMMATRIX projMat;
	POINT mousePosition;

	GetCursorPos(&mousePosition);
	ScreenToClient(m_hwnd, &mousePosition);

	auto mouseX = mousePosition.x;
	auto mouseY = mousePosition.y;

	DirectX::XMVECTOR pickRayInViewSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR pickRayInViewSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;

	//float pointX = ((2.f * mousePosition.x) / 1280) - 1.f;
	//float pointY = (((2.f * mousePosition.y) / 800) - 1.f) * -1.f;

	//Transform 2D pick position on screen space to 3D ray in View space
	PRVecX = (((2.0f * mouseX) / 1280) - 1);
	PRVecY = -(((2.0f * mouseY) / 800) - 1);
	PRVecZ = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	DirectX::XMFLOAT4X4 storeProjMat;
	m_pDXManager->GetProjectionMatrix(projMat);
	DirectX::XMStoreFloat4x4(&storeProjMat, projMat);
	PRVecX /= storeProjMat._11;
	PRVecY /= storeProjMat._22;

	pickRayInViewSpaceDir = DirectX::XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	//Uncomment this line if you want to use the center of the screen (client area)
	//to be the point that creates the picking ray (eg. first person shooter)
	//pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	DirectX::XMMATRIX pickRayToWorldSpaceMatrix;
	DirectX::XMVECTOR matInvDeter;    //We don't use this, but the xna matrix inverse function requires the first parameter to not be null

	pickRayToWorldSpaceMatrix = DirectX::XMMatrixInverse(&matInvDeter, m_pCamera->GetViewMatrix());    //Inverse of View Space matrix is World space matrix

	auto pickRayInWorldSpacePos = DirectX::XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	auto pickRayInWorldSpaceDir = DirectX::XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
	
	
	DirectX::XMFLOAT4 storeRayDir;
	DirectX::XMStoreFloat4(&storeRayDir, pickRayInWorldSpaceDir);

		 storeRayDir.x *= 200.f;
		 storeRayDir.y *= 200.f;
		 storeRayDir.z = m_pCamera->GetPosition().z + 500.f;
		 storeRayDir.w = 0.f;

	//std::cout << " New vector is" <<
	//	storeRayDir.x << ", " <<
	//	storeRayDir.y << ", " <<
	//	storeRayDir.z << ", " << std::endl;


	 if(m_pInput->IsLeftClickPressed())
	 {
		 m_pSwarmManager->SetGoalPosition(storeRayDir);
		//
		//std::cout << "Camera position and thus Goal Position is " <<
		//	 pointX << ", " <<
		//	 pointY << ", " <<
		//	 0.f << ", " << std::endl;
	 }




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

	viewMat = m_pCamera->GetViewMatrix();
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