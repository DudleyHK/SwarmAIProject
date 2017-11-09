/*



*/
#pragma once

#include <memory>
#include <windows.h>

#include "DXManager.h"
#include "Camera.h"
#include "ColourShaderManager.h"
#include "ModelManager.h"


const bool  g_FULL_SCREEN = false;
const bool  g_VSYNC_ENABLED = true;
const float g_SCREEN_DEPTH = 1000.0f;
const float g_SCREEN_NEAR = 0.1f;

class GraphicsManager
{
public:
	GraphicsManager() = default;
	// GraphicsManager(const GraphicsManager&);
	~GraphicsManager() = default;

	bool Init(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Update();



private:
	bool Render();

	std::unique_ptr<DXManager> m_pDXManager = nullptr;
	std::unique_ptr<Camera> m_pCamera = nullptr;
	std::unique_ptr<ModelManager> m_pModelManager = nullptr;
	std::unique_ptr<ColourShaderManager> m_pColShaderManager = nullptr;

	
};