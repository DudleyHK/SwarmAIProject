/*



*/
#include "Particle.h"


const Vector3 ConvertToVector3(const DirectX::XMFLOAT3&);


Particle::Particle() :
	m_mass(1.0f),
	m_speed(0.0f),
	m_radius(0.1f),
	m_position({0.f, 0.f, 0.f}),
	m_velocity({0.f, 0.f, 0.f}),
	m_forces({0.f, 0.f, 0.f}),
	m_gravity({0.f, 0.f, 0.f})

{
	m_gravity.y = m_mass * GRAVITY_ACCELERATION;
}

const Vector3 Particle::GetPosition() const
{
	return m_position;
}

const Vector3 Particle::GetVelocity() const
{
	return m_velocity;
}

const float Particle::GetMass() const
{
	return m_mass;
}

void Particle::SetPosition(const Vector3 position)
{
	m_position = position;
}

void Particle::SetVelocity(const Vector3 velocity)
{
	m_velocity = velocity;
}

void Particle::CalcLoads()
{
	CalcLoads(Vector3::Zero());
}
void Particle::CalcLoads(DirectX::XMFLOAT3 force)
{
	CalcLoads(Vector3(force.x, force.y, force.z));
}
void Particle::CalcLoads(Vector3 force)
{
	// reset forces
	m_forces.x = 0.f;
	m_forces.y = 0.f;
	m_forces.z = 0.f;

	// Aggregatae forces
	m_forces += m_gravity + force;
	//m_forces += force;
}



void Particle::UpdateBodyEuler(float dt)
{
	Vector3 acc;
	Vector3 deltaVel;
	Vector3 deltaSpeed;

	// Integrate equation of motion.
	acc = m_forces / m_mass;
	deltaVel = acc * dt;
	m_velocity += deltaVel;

	deltaSpeed = m_velocity * dt;
	m_position += deltaSpeed;

	// Misc. calculations
	m_speed = m_velocity.Magnitude();
}


const Vector3 Particle::ConvertToVector3(const DirectX::XMFLOAT3& convert)
{
	return Vector3(convert.x, convert.y, convert.z);
}
