/*



*/
#include "Position.h"

void Position::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}

void Position::SetRotation(DirectX::XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

DirectX::XMFLOAT3 Position::GetPosition()
{
	return m_position;
}

DirectX::XMFLOAT3 Position::GetRotation()
{
	return m_rotation;
}

void Position::SetFrameTime(float time)
{ 
	m_frameTime = time;
}

void Position::SetSpeedMultiplier(bool multiply)
{
	if(multiply && !m_multiplierLock)
	{
		m_horAcceleration *= m_speedMultiplier;
		m_horMaxSpeed     *= m_speedMultiplier;
		m_horCoolDown     *= m_speedMultiplier;
					      
		m_vertAcceleration *= m_speedMultiplier;
		m_vertMaxSpeed *= m_speedMultiplier;
		m_vertCoolDown *= m_speedMultiplier;

		m_rotAcceleration *= m_speedMultiplier;
		m_rotMaxSpeed *= m_speedMultiplier;
		m_rotCoolDown *= m_speedMultiplier;
		m_multiplierLock = true;
	}
	else
	{
		// Set to original Speeds
		m_horAcceleration = 1.f;
		m_horMaxSpeed = 3.f;
		m_horCoolDown = 0.5f;

		m_vertAcceleration = 1.5f;
		m_vertMaxSpeed = 3.5f;
		m_vertCoolDown = 1.f;

		m_rotAcceleration = 2.5f;
		m_rotMaxSpeed = 5.f;
		m_rotCoolDown = 1.f;

		m_multiplierLock = false;
	}
}



void Position::MoveForward(bool keydown)
{ 
	if(keydown)
	{
		//m_forwardSpeed += m_frameTime * 0.001f;
		m_forwardSpeed += m_frameTime * m_horAcceleration;

		if(m_forwardSpeed > (m_frameTime * m_horMaxSpeed))
		{
			m_forwardSpeed = m_frameTime * m_horMaxSpeed;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * m_horCoolDown;

		if(m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	auto radians = DirectX::XMConvertToRadians(m_rotation.y);

	// Update the position.
	m_position.x += sinf(radians) * m_forwardSpeed;
	m_position.z += cosf(radians) * m_forwardSpeed;
}

void Position::MoveBackward(bool keydown)
{
	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_backwardSpeed += m_frameTime * m_horAcceleration;

		if(m_backwardSpeed > (m_frameTime * m_horMaxSpeed))
		{
			m_backwardSpeed = m_frameTime * m_horMaxSpeed;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * m_horCoolDown;

		if(m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	auto radians = DirectX::XMConvertToRadians(m_rotation.y);

	// Update the position.
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;
}

void Position::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_upwardSpeed += m_frameTime * m_vertAcceleration;

		if(m_upwardSpeed > (m_frameTime * m_vertMaxSpeed))
		{
			m_upwardSpeed = m_frameTime * m_vertMaxSpeed;
		}
	}
	else
	{
		m_upwardSpeed -= m_frameTime * m_vertCoolDown;

		if(m_upwardSpeed < 0.0f)
		{
			m_upwardSpeed = 0.0f;
		}
	}

	m_position.y += m_upwardSpeed;
}

void Position::MoveDownward(bool keydown)
{
	if(keydown)
	{
		m_downwardSpeed += m_frameTime * m_vertAcceleration;

		if(m_downwardSpeed > (m_frameTime * m_vertMaxSpeed))
		{
			m_downwardSpeed = m_frameTime * m_vertMaxSpeed;
		}
	}
	else
	{
		m_downwardSpeed -= m_frameTime * m_vertCoolDown;

		if(m_downwardSpeed < 0.0f)
		{
			m_downwardSpeed = 0.0f;
		}
	}

	m_position.y -= m_downwardSpeed;
}

void Position::TurnLeft(bool keydown)
{
	if(keydown)
	{
		m_leftTurnSpeed += m_frameTime * m_rotAcceleration;

		if(m_leftTurnSpeed > (m_frameTime * m_rotMaxSpeed))
		{
			m_leftTurnSpeed = m_frameTime * m_rotMaxSpeed;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * m_rotCoolDown;

		if(m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	m_rotation.y -= m_leftTurnSpeed;

	if(m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}
}

void Position::TurnRight(bool keydown)
{
	if(keydown)
	{
		m_rightTurnSpeed += m_frameTime * m_rotAcceleration;

		if(m_rightTurnSpeed > (m_frameTime * m_rotMaxSpeed))
		{
			m_rightTurnSpeed = m_frameTime * m_rotMaxSpeed;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * m_rotCoolDown;

		if(m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.y += m_rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}
}

void Position::LookUpward(bool keydown)
{
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * m_rotAcceleration;

		if(m_lookUpSpeed > (m_frameTime * m_rotMaxSpeed))
		{
			m_lookUpSpeed = m_frameTime * m_rotMaxSpeed;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime * m_rotCoolDown;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.x -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}
}

void Position::LookDownward(bool keydown)
{
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * m_rotAcceleration;

		if(m_lookDownSpeed > (m_frameTime * m_rotMaxSpeed))
		{
			m_lookDownSpeed = m_frameTime * m_rotMaxSpeed;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime * m_rotCoolDown;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotation.x += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}
}
