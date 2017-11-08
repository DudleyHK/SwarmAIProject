/*



*/
#include "SystemManager.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
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