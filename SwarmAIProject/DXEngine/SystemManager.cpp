/*



*/
#include "SystemManager.h"



bool SystemManager::Init()
{
	int screenWidth = 0;
	int screenHeight = 0;

	InitWindows(screenWidth, screenHeight);




	m_pApplication = std::make_unique<Application>();
	if (!m_pApplication)
	{
		return false;
	}

	if (!m_pApplication->Init(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		return false;
	}
	return true;
}

void SystemManager::Shutdown()
{
	if (m_pApplication)
	{
		m_pApplication->Shutdown();
	}

	ShutdownWindow();
}

void SystemManager::Run()
{
	MSG msg = {0};
	ZeroMemory(&msg, sizeof(MSG));

	bool done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			if (!Update())
			{
				done = true;
			}
		}
	}
}


bool SystemManager::Update()
{
	// All of this should live in the ApplicationClass.
	
	if (!m_pApplication->Update())
	{
		return false;
	}

	if(!m_pApplication->Render())
	{
		return false;
	}
	return true;
}





LRESULT CALLBACK SystemManager::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemManager::InitWindows(int& screenWidth, int& screenHeight)
{
	int posX = 0;
	int posY = 0;

	ApplicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"Swarm Intelligence Research Project";

	WNDCLASSEX wc = {0};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// check whether fullscreen setup is required.
	if (g_FULL_SCREEN)
	{
		DEVMODE dmScreenSettings = {0};
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = 0;
		posY = 0;
	}
	else
	{
		screenWidth = 1280;
		screenHeight = 800;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemManager::ShutdownWindow()
{
	if (g_FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;
	ApplicationHandle = nullptr;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_DESTROY: PostQuitMessage(0);
			return 0;

		case WM_CLOSE: PostQuitMessage(0);
			return 0;

		default:
			return ApplicationHandle->MessageHandler(hwnd, msg, wparam, lparam);
	}
}