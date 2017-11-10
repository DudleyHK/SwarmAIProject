/*



*/
#pragma once


#include <DirectXMath.h>

class Position
{
public:
	Position()  = default;
	~Position() = default;

	void SetPosition(DirectX::XMFLOAT3);
	void SetRotation(DirectX::XMFLOAT3);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();

	void SetFrameTime(float);
	void SetSpeedMultiplier(bool);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void LookUpward(bool);
	void LookDownward(bool);

private:

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;

	float m_speedMultiplier = 7.5f;
	bool m_multiplierLock = false;

	float m_horAcceleration = 1.f;
	float m_horMaxSpeed   = 3.f;
	float m_horCoolDown   = 0.5f;
	float m_vertAcceleration = 1.5f;
	float m_vertMaxSpeed     = 3.5f;
	float m_vertCoolDown     = 1.f;
	float m_rotAcceleration = 2.5f;
	float m_rotMaxSpeed     = 5.f;
	float m_rotCoolDown     = 1.f;


	float m_frameTime = 0.1f;

	float m_forwardSpeed   = 0.f;
	float m_backwardSpeed  = 0.f;
	float m_upwardSpeed    = 0.f;
	float m_downwardSpeed  = 0.f;
	float m_leftTurnSpeed  = 0.f;
	float m_rightTurnSpeed = 0.f;
	float m_lookUpSpeed    = 0.f;
	float m_lookDownSpeed  = 0.f;

};