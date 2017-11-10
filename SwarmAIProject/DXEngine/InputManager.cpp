/*



*/
#include "InputManager.h"
#include "Utilities.h"



bool InputManager::Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	auto result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void InputManager::Shutdown()
{
	if(m_mouse)
	{
		m_mouse->Unacquire();
		SafeRelease(m_mouse);
	}
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		SafeRelease(m_keyboard);
	}
	SafeRelease(m_directInput);

}

bool InputManager::Update()
{
	auto result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	ProcessInput();

	return true;
}



DirectX::XMINT2 InputManager::GetMouseLocation()
{
	return m_mouseLocation;
}


bool InputManager::ReadKeyboard()
{
	// Read the keyboard device.
	auto result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool InputManager::ReadMouse()
{
	auto result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

void InputManager::ProcessInput()
{
	m_mouseLocation.x += m_mouseState.lX;
	m_mouseLocation.y += m_mouseState.lY;

	if(m_mouseLocation.x < 0)
	{
		m_mouseLocation.x = 0;
	}
	if(m_mouseLocation.y < 0)
	{
		m_mouseLocation.y = 0;
	}

	if(m_mouseLocation.x > m_screenWidth)
	{
		m_mouseLocation.x = m_screenWidth;
	}
	if(m_mouseLocation.y > m_screenHeight)
	{
		m_mouseLocation.y = m_screenHeight;
	}
}

bool InputManager::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsLeftPressed()
{
	if(m_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(m_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsWPressed()
{
	if(m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}
	return false;
}


bool InputManager::IsAPressed()
{
	if(m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsSPressed()
{
	if(m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}
	return false;
}


bool InputManager::IsDPressed()
{
	if(m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsEPressed()
{
	if(m_keyboardState[DIK_E] & 0x80)
	{
		return true;
	}

	return false;
}



bool InputManager::IsQPressed()
{
	if(m_keyboardState[DIK_Q] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputManager::IsZPressed()
{
	if(m_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgUpPressed()
{
	if(m_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsPgDownPressed()
{
	if(m_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}


bool InputManager::IsLShiftPressed()
{
	if(m_keyboardState[DIK_LSHIFT] & 0x80)
	{
		return true;
	}

	return false;
}