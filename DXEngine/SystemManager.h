/*



*/
#pragma once
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <Windows.h>

#include "InputManager.h"
#include "GraphicsManager.h"


class SystemManager
{
public:
	SystemManager() = default;
	//SystemManager(const SystemManager&);
	~SystemManager() = default;

	bool Init();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitWindows(int&, int&);
	void ShutdownWindow();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	std::unique_ptr<InputManager>    m_pInput = nullptr;
	std::unique_ptr<GraphicsManager> m_pGraphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemManager* ApplicationHandle = nullptr;
  