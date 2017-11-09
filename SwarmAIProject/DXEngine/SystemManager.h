/*



*/
#pragma once
#define WIN32_LEAN_AND_MEAN

#include <memory>
#include <Windows.h>


#include "Application.h"


class SystemManager
{
public:
	SystemManager() = default;
	//SystemManager(const SystemManager&);
	~SystemManager() = default;

	bool Init();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	bool Update();
	void InitWindows(int& screenWidth, int& screenHeight);
	void ShutdownWindow();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	std::unique_ptr<Application>  m_pApplication = nullptr;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static SystemManager* ApplicationHandle = nullptr;
  