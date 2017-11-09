/*



*/
#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <DirectXMath.h>




class InputManager
{
public:
	InputManager() = default;
	~InputManager() = default;

	bool Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	void Shutdown();
	bool Update();

	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsAPressed();
	bool IsZPressed();
	bool IsPgUpPressed();
	bool IsPgDownPressed();

	DirectX::XMINT2 InputManager::GetMouseLocation();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth;
	int m_screenHeight;

	DirectX::XMINT2 m_mouseLocation = {0, 0};
};