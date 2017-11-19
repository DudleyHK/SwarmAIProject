/*



*/
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


class Rigidbody
{
public:
	Rigidbody() = default;
	~Rigidbody() = default;

private:
	DirectX::XMVECTOR m_velocity;
	float mass;
};