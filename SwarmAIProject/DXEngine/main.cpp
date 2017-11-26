/*



*/
#include "SystemManager.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	//add a Debug Console to which cout is directed whilst in DEBUG
	if(AllocConsole())
	{
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle(L"Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
#endif



	std::unique_ptr<SystemManager> m_pSystemManager = std::make_unique<SystemManager>();

	if (!m_pSystemManager)
	{
		return 0;
	}

	if (m_pSystemManager->Init())
	{
		m_pSystemManager->Run();
	}

	m_pSystemManager->Shutdown();
	return 0;
}