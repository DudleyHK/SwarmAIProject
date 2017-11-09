/*



*/
#pragma once

#include <memory>
#include <windows.h>

#include "DXManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "ColourShaderManager.h"
#include "ModelManager.h"
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

	bool HandleInput(float frameTime);

	std::unique_ptr<DXManager> m_pDXManager = nullptr;
	std::unique_ptr<Camera> m_pCamera = nullptr;
	std::unique_ptr<InputManager> m_pInput = nullptr;
	std::unique_ptr<Position> m_pPosition = nullptr;
	std::unique_ptr<ModelManager> m_pModelManager = nullptr;
	std::unique_ptr<ColourShaderManager> m_pColShaderManager = nullptr;

	
};