/*



*/
#pragma once

#include <memory>
#include <windows.h>

#include "DXManager.h"

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

	bool Init(int, int, HWND);
	void Shutdown();
	bool Update();



private:
	bool Render();

	std::unique_ptr<DXManager> m_pDXManager = nullptr;

	
};