/*



*/
#pragma once
#include <vector>

#include "Vector3.h"



class Particle
{
public:

	Particle();
	~Particle() = default;

	std::vector<Particle*>*   GetNeighbours();
	const Vector3   GetSeperationForce() const;
	const Vector3   GetPosition()   const;
	const Vector3   GetVelocity()   const;
	const float     GetMass()       const;
	const int       GetRadius()     const;
	const int       GetID()         const;
	void SetSeperationForce(const Vector3);
	void SetPosition(const Vector3);
	void SetVelocity(const Vector3);
	void SetID(const int);

	void AddNeighbour(Particle*);
	void ClearNeighbours();

	void CalcLoads();
	void CalcLoads(DirectX::XMFLOAT3 force);
	void CalcLoads(Vector3 force);

	void UpdateBodyEuler(float dt);
	static Vector3 ConvertToVector3(const DirectX::XMFLOAT3&);



public:
	int m_ID = -1;
	float m_mass;
	float m_speed;
	float m_radius;
	Vector3 m_position;
	Vector3 m_velocity;
	Vector3 m_forces;
	Vector3 m_gravity;
	Vector3 m_seperationForce;

	std::vector<Particle*> neighbours;

	const float GRAVITY_ACCELERATION = -9.8f;

};