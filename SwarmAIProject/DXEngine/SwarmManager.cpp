/*



*/
#include <iostream>
#include <random>

#include "SwarmManager.h"




const bool SwarmManager::Init(int instanceCount)
{
	m_instanceCount = instanceCount;

	m_Particles.reserve(instanceCount);
	m_WorldMatrices.reserve(instanceCount);

	// give each particle a random veloctiy and position;
	for(auto i = 0; i < m_instanceCount; i++)
	{
		auto randPosition = GetRandomPosition();
		auto worldMat = DirectX::XMMatrixTranslation(randPosition.x, randPosition.y, randPosition.z);
		worldMat = DirectX::XMMatrixTranspose(worldMat);


		m_WorldMatrices.push_back(std::make_unique<DirectX::XMMATRIX>(worldMat));
		m_Particles.push_back(std::make_unique<Particle>(randPosition));
	}

	// Set a random global best distance to get it started.
	m_globalBestDistance = CalculateDistance(m_Particles[0]->m_position, GOAL_POSITION);

	return false;
}

void SwarmManager::Update()
{
	SetGlobalBestDistance();
	CalculateParticlePhysics();
}

DirectX::XMMATRIX* SwarmManager::GetWorldMatrices()
{
	return m_WorldMatrices.data()->get();
}


// Calculate global best position 
void SwarmManager::SetGlobalBestDistance()
{
	//std::cout << "Global Best is " << 
	//	m_globalBestPosition.x << ", " << 
	//	m_globalBestPosition.y << ", " << 
	//	m_globalBestPosition.z << ", " << std::endl;
   //
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

		particle->m_velocity.x += acceleration.x * m_deltaTime; // dt
		particle->m_velocity.y += acceleration.y * m_deltaTime; // dt
		particle->m_velocity.z += acceleration.z * m_deltaTime; // dt

		particle->m_position.x += particle->m_velocity.x * m_deltaTime; // dt
		particle->m_position.y += particle->m_velocity.y * m_deltaTime; // dt
		particle->m_position.z += particle->m_velocity.z * m_deltaTime; // dt


		//std::cout << "Instance position of particle " << i << " is " <<
		//	particle->m_position.x << ", " <<
		//	particle->m_position.y << ", " <<
		//	particle->m_position.z << ", " << std::endl;
		//
		//
		//std::cout << "Instance velocity of particle " << i << " is " <<
		//	particle->m_velocity.x << ", " <<
		//	particle->m_velocity.y << ", " <<
		//	particle->m_velocity.z << ", " << std::endl;



		UpdateWorldMatrix(particle, i);
	}
}



void SwarmManager::UpdateWorldMatrix(const Particle* particle, const int index)
{
	auto worldMatrix  = *m_WorldMatrices[index].get();
	auto pPos = particle->m_position;
	
	// static float rotation = 0.f;
	//
	//rotation += (float)DirectX::XM_PI * 0.005f;
	//if(rotation > 360.f)
	//{
	//	rotation -= 360.f;
	//}
	//if(index == 0)
	//{
	//	worldMatrix = DirectX::XMMatrixRotationY(rotation);
	//}
	
	worldMatrix = DirectX::XMMatrixTranslation(pPos.x, pPos.y, pPos.z);
	*m_WorldMatrices[index].get() = worldMatrix;

}



const DirectX::XMFLOAT3 SwarmManager::GetRandomPosition()
{
	std::random_device randDevice;
	std::mt19937 generate(randDevice());  
	std::uniform_real_distribution<> distribute(static_cast<double>(-5), static_cast<double>(5)); // both inclusive


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

	return  x + y + z;//(x * x) + (y * y) + (z * z);
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
