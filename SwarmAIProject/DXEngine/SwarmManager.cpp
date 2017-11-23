/*



*/
#include <random>

#include "SwarmManager.h"




const bool SwarmManager::Init(int instanceCount)
{
	m_Particles.resize(instanceCount);
	m_instanceCount = instanceCount;

	// give each particle a random veloctiy and position;
	for(auto& particle : m_Particles)
	{
		auto randPosition = GetRandomPosition();
		particle = std::make_unique<Particle>(randPosition);
	}

	// Set a random global best distance to get it started.
	m_globalBestDistance = CalculateDistance(m_Particles[0]->m_position, GOAL_POSITION);

	return false;
}

void SwarmManager::Update(DirectX::XMMATRIX* instanceMatrices)
{
	m_instanceMatrices = instanceMatrices;

	SetGlobalBestDistance();
	CalculateParticlePhysics();
}

DirectX::XMMATRIX* SwarmManager::GetWorldMatrices()
{
	return m_instanceMatrices;
}


// Calculate global best position 
void SwarmManager::SetGlobalBestDistance()
{
	for(auto i = 0; i < m_instanceCount; i++)
	{
		auto particle = m_Particles[i].get();
		auto particleDist = CalculateDistance(particle->m_position, m_globalBestPosition);
		if(particleDist < m_globalBestDistance)
		{
			m_globalBestDistance = particleDist;
			m_globalBestPosition = particle->m_position;
		}
	}
}


// Run this through the profiler
void SwarmManager::CalculateParticlePhysics()
{
	// Calculate Particle Physics
	for(auto i = 0; i < m_instanceCount; i++)
	{
		auto particle = m_Particles[i].get();
		auto direction = CalculateDirection(particle->m_position, m_globalBestPosition);
		auto normalisedDirection = NormaliseFloat3(direction);

		auto force = ComputeForce(normalisedDirection, particle->m_mass);
		auto acceleration = CalculateAcceleration(force, particle->m_mass);

		particle->m_velocity.x += acceleration.x * 0.001f; // dt
		particle->m_velocity.y += acceleration.y * 0.001f; // dt
		particle->m_velocity.z += acceleration.z * 0.001f; // dt

		particle->m_position.x += particle->m_velocity.x * 0.001f; // dt
		particle->m_position.y += particle->m_velocity.y * 0.001f; // dt
		particle->m_position.z += particle->m_velocity.z * 0.001f; // dt

		UpdateWorldMatrix(particle, i);
	}
}



void SwarmManager::UpdateWorldMatrix(const Particle* particle, const int index)
{
	//auto worldMatrix = m_instanceMatrices[index];
	//auto pPos = particle->m_position;
	//
	////DirectX::XMVECTOR scale;
	////DirectX::XMVECTOR rotation;
	////DirectX::XMVECTOR translation;
	////
	////DirectX::XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);
	//
	//
	//auto transMat = DirectX::XMMatrixTranslation(pPos.x, pPos.y, pPos.z);
	//worldMatrix = DirectX::XMMatrixTranspose(transMat);
	//int i = 0;
}



const DirectX::XMFLOAT3 SwarmManager::GetRandomPosition()
{
	std::random_device randDevice;
	std::mt19937 generate(randDevice());  
	std::uniform_real_distribution<> distribute(static_cast<double>(-10), static_cast<double>(10)); // both inclusive


	auto x = static_cast<float>(distribute(generate));
	auto y = static_cast<float>(distribute(generate));
	auto z = static_cast<float>(distribute(generate));

	return {x, y, z};

}



const float SwarmManager::CalculateDistance(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b)
{
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	float z = (b.z - a.z);

	return sqrtf((x * x) + (y * y) + (z * z)); 
}



const DirectX::XMFLOAT3 SwarmManager::CalculateDirection(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b)
{
	auto x = b.x - a.x;
	auto y = b.y - a.y;
	auto z = b.z - a.z;

	return DirectX::XMFLOAT3(x, y, z);
}


const DirectX::XMFLOAT3 SwarmManager::NormaliseFloat3(const DirectX::XMFLOAT3 a)
{
	auto aVec = DirectX::XMLoadFloat3(&a);
	auto aVecNormalised = DirectX::XMVector3Normalize(aVec);

	DirectX::XMFLOAT3 aNormalised;
	DirectX::XMStoreFloat3(&aNormalised, aVecNormalised);

	return aNormalised;
}


const DirectX::XMFLOAT3 SwarmManager::ComputeForce(const DirectX::XMFLOAT3 dir, const float mass)
{
	auto force = 30.f;

	auto x = (dir.x * force) * mass;
	auto y = (dir.y * force) * mass;
	auto z = (dir.z * force) * mass;


	return DirectX::XMFLOAT3(x, y, z);
}

const DirectX::XMFLOAT3 SwarmManager::CalculateAcceleration(const DirectX::XMFLOAT3 force, const float mass)
{
	auto x = force.x / mass;
	auto y = force.y / mass;
	auto z = force.z / mass;

	return DirectX::XMFLOAT3(x, y, z);
}
