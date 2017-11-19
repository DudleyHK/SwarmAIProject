/*
*/
#pragma once

#include <memory>
#include <vector>
#include <windows.h>


#include "DXManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "ColourShaderManager.h"
#include "Model.h"
#include "Position.h"


const bool  g_FULL_SCREEN = false;
const bool  g_VSYNC_ENABLED = true;
const float g_SCREEN_DEPTH = 1000.0f;
const float g_SCREEN_NEAR = 0.1f;

class Application
{
public:
	Application() = default;
	~Application() = default;

	bool Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	bool Update();
	bool Render();
	void Shutdown();


private:

	void UpdateSwarm();
	float CalculateDistance(Model*);
	DirectX::XMFLOAT3 CalculateParticleVelocity(Model*);
	DirectX::XMFLOAT3 UpdateParticlePosition(DirectX::XMFLOAT3);
	bool RenderSwarm(DirectX::XMMATRIX& viewMat, DirectX::XMMATRIX& projMat);
	bool HandleInput(float frameTime);

	std::unique_ptr<DXManager> m_pDXManager = nullptr;
	std::unique_ptr<Camera> m_pCamera = nullptr;
	std::unique_ptr<InputManager> m_pInput = nullptr;
	std::unique_ptr<Position> m_pPosition = nullptr;
	//std::unique_ptr<Model> m_pModel = nullptr;
	std::unique_ptr<ColourShaderManager> m_pColShaderManager = nullptr;

	std::vector<std::unique_ptr<Model>> m_ModelList;

	DirectX::XMFLOAT3 m_goalPosition = {100.f, 100.f, 100.f};
	DirectX::XMFLOAT3 m_globalBestPosition;
	float m_globalBestDistance;

};