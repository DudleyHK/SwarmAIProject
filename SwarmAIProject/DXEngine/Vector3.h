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
		return (x * x) + (y * y) + (z * z); // sqrtf removed to avoid performance hit.
	}

	float x;
	float y;
	float z;

	bool operator!=    (const Vector3& vc3)
	{
		return ((x != vc3.x) &&
			(y != vc3.y) &&
				(z != vc3.z));
	}
	Vector3 operator+= (const Vector3& vc3)
	{
		return Vector3(
			x += vc3.x,
			y += vc3.y,
			z += vc3.z);
	}
	Vector3 operator+  (const Vector3& vc3)
	{
		return Vector3(
			x + vc3.x,
			y + vc3.y,
			z + vc3.z);
	}
	Vector3 operator/  (const float& sc)
	{
		return Vector3(x / sc, y / sc, z / sc);
	}
	Vector3 operator*  (const float& sc)
	{
		return Vector3(x * sc, y * sc, z * sc);
	}

	static Vector3 Zero()
	{
		return Vector3();
	}
};

Vector3 ConvertToVector3(const DirectX::XMFLOAT3&);