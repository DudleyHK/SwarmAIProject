#include "InputManager.h"




void InputManager::Init()
{
	for (auto i = 0; i<256; i++)
	{
		m_keys[i] = false;
	}
}


void InputManager::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}


void InputManager::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}


bool InputManager::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}
