/*



*/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

struct Vector3
{
	Vector3(float _x = 0.f, float _y = 0.f, float _z = 0.f)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	DirectX::XMFLOAT3 DXFloat3() const
	{
		return DirectX::XMFLOAT3(x, y, z);
	}
	DirectX::XMVECTOR DXVector() const
	{
		return DirectX::XMLoadFloat3(&DXFloat3());
	}
	float Magnitude()
	{
		return sqrtf((x * x) + (y * y) + (z * z));
	}

	float x;
	float y;
	float z;


	Vector3 operator+= (const Vector3& vc3)
	{
		return Vector3(
			x += vc3.x,
			y += vc3.y,
			z += vc3.z);
	}
	Vector3 operator+ (const Vector3& vc3)
	{
		return Vector3(
			x + vc3.x, 
			y + vc3.y, 
			z + vc3.z);
	}
	Vector3 operator/ (const float& sc)
	{
		return Vector3(x / sc, y / sc, z / sc);
	}
	Vector3 operator* (const float& sc)
	{
		return Vector3(x * sc, y * sc, z * sc);
	}

	static Vector3 Zero()
	{
		return Vector3();
	}
};


class Particle
{
public:

	Particle();
	~Particle() = default;

	const Vector3 GetPosition() const;
	const Vector3 GetVelocity() const;
	const float   GetMass() const;
	void SetPosition(const Vector3);
	void SetVelocity(const Vector3);

	void CalcLoads();
	void CalcLoads(DirectX::XMFLOAT3 force);
	void CalcLoads(Vector3 force);

	void UpdateBodyEuler(float dt);
	static const Vector3 ConvertToVector3(const DirectX::XMFLOAT3&);



private:
	float m_mass;
	float m_speed;
	float m_radius;
	Vector3 m_position;
	Vector3 m_velocity;
	Vector3 m_forces;
	Vector3 m_gravity;

	const float GRAVITY_ACCELERATION = -9.8f;

};