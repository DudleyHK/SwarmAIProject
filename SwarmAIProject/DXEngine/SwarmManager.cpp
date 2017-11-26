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
		m_WorldMatrices.push_back(std::make_unique<DirectX::XMMATRIX>(worldMat));

		m_Particles.push_back(std::make_unique<Particle>());
		m_Particles[i]->SetPosition(Particle::ConvertToVector3(randPosition));


		//std::cout << "Initial position of Particle " << i << " is " <<
		//	randPosition.x << ", " <<
		//	randPosition.y << ", " <<
		//	randPosition.z << ", " << std::endl;

	}

	// Set a random global best distance to get it started.
	m_globalBestDistance = CalculateDistance(m_Particles[0]->GetPosition().DXFloat3(), GOAL_POSITION);
	m_globalBestPosition = m_Particles[0]->GetPosition().DXFloat3();
	
	return false;
}

void SwarmManager::Update()
{
	SetGlobalBestDistance();
	CalculateParticlePhysics();
}

const DirectX::XMMATRIX& SwarmManager::GetWorldAt(const int index) const
{
	auto worldMat = m_WorldMatrices[index].get();

	if(!worldMat)
	{
		std::cout << "\n\nERROR: WorldMatrix is null in GetWorldAt() in SwarmManager class \n\n" << std::endl;
	}

	return *worldMat;

}

DirectX::XMMATRIX* SwarmManager::GetWorldMatrices()
{
	return m_WorldMatrices.data()->get();
}


// Calculate global best position for each particle to move towards.
void SwarmManager::SetGlobalBestDistance()
{
	for(auto i = 0; i < m_instanceCount; i++)
	{
		auto particle = m_Particles[i].get();
		auto particleDist = CalculateDistance(particle->GetPosition().DXFloat3(), GOAL_POSITION);

		//std::cout << "Particle distance of particle " << i << " is " <<
		//	particleDist;
		//std::cout << " ||| Global Best Distance is " << m_globalBestDistance;
		//std::cout << " ||| Global Best Position is " << " (" << 
		//	m_globalBestPosition.x << ", " <<
		//	m_globalBestPosition.y << ", " <<
		//	m_globalBestPosition.z << ") " << std::endl;

		if(particleDist < m_globalBestDistance)
		{
			m_globalBestDistance = particleDist;
			m_globalBestPosition = particle->GetPosition().DXFloat3();
		 
			//std::cout << "Particle distance of particle " << i << " is " <<
			//	particleDist;
			//std::cout << " ||| Global Best Distance is " << m_globalBestDistance;
			//std::cout << " ||| Global Best Position is " << " (" <<
			//	m_globalBestPosition.x << ", " <<
			//	m_globalBestPosition.y << ", " <<
			//	m_globalBestPosition.z << ") " << std::endl;
			//
			//std::cout << "BEST GLOBAL UPDATED" << std::endl << std::endl;
		} 
	}
}


// Run this through the profiler
void SwarmManager::CalculateParticlePhysics()
{
	// Calculate Particle Physics
	for(int i = 0; i < m_instanceCount; i++)
	{
		auto particle = m_Particles[i].get();
		auto direction = CalculateDirection(particle->GetPosition().DXFloat3(), m_globalBestPosition);
		auto normalisedDirection = NormaliseFloat3(direction);
		auto force = ComputeForce(normalisedDirection, particle->GetMass());

		particle->CalcLoads(force);
		particle->UpdateBodyEuler(m_deltaTime);

		UpdateWorldMatrix(particle, i);
	}
}



void SwarmManager::UpdateWorldMatrix(const Particle* particle, const int index)
{
	auto pPos = particle->GetPosition();
	auto freshWorld = DirectX::XMMatrixIdentity();
	//std::cout << "Instance position of particle " << index << " is " <<
	//	particle->m_position.x << ", " <<
	//	particle->m_position.y << ", " <<
	//	particle->m_position.z << ", " << std::endl;
	

	auto translation = DirectX::XMMatrixTranslation(pPos.x, pPos.y, pPos.z);
	freshWorld = DirectX::XMMatrixTranspose(translation);

	*m_WorldMatrices[index].get() = freshWorld;

}



const DirectX::XMFLOAT3 SwarmManager::GetRandomPosition()
{
	std::random_device randDevice;
	std::mt19937 generate(randDevice());  
	std::uniform_real_distribution<> distribute(static_cast<double>(-100), static_cast<double>(100)); // both inclusive


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

	return (x * x) + (y * y) + (z * z); // remove sqrt because its performance heavy.
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
	auto force = 25.f;

	auto x = (dir.x * force) * mass;
	auto y = (dir.y * force) * mass;
	auto z = (dir.z * force) * mass;


	return DirectX::XMFLOAT3(x, y, z);
}