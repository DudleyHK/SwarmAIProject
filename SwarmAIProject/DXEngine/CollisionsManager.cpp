/*



*/
#include <iostream>

#include "CollisionsManager.h"
#include "Particle.h"


const bool CollisionsManager::Init(const int instanceCount)
{
	// reserve vector space. 
	m_Positions.reserve(totalSegments);
	m_ActiveParticles.reserve(instanceCount);

	m_CollisionSegments.resize((totalSegments * totalSegments * totalSegments));
	for(auto segment : m_CollisionSegments)
	{
		segment.reserve(instanceCount / (totalSegments * totalSegments * totalSegments));
	}

	PartitionWorld();


	return false;
}



void CollisionsManager::AddParticle(Particle* particles)
{
	//float prevX = 0.f;
	//float prevY = 0.f;
	//float prevZ = 0.f;
	////for(auto& segmentPos : m_Positions)
	//{
	//	if(particles->GetPosition().x > segmentPos.x)
	//	{
	//		prevX = segmentPos.x;
	//	}
	//
	//	if(particles->GetPosition().y > segmentPos.y)
	//	{
	//		prevY = segmentPos.y;
	//	}
	//
	//	if(particles->GetPosition().z> segmentPos.z)
	//	{
	//		prevZ = segmentPos.z;
	//	}
	//}
	//
	//for(auto i = 0; i < m_Positions.size(); i++)
	//{
	//	auto segmentPos = m_Positions[i];
	//	if((segmentPos.x == prevX) && (segmentPos.y == prevY) && (segmentPos.z == prevZ))
	//	{
	//		m_CollisionSegments[i].push_back(particles);
	//	}
	//}

}



void CollisionsManager::Update()
{




	ResetCollisionManager();
}

void CollisionsManager::PartitionWorld()
{
	auto xSegments = (maxPosition.x - minPosition.x) / totalSegments; // size of each segment.
	auto ySegments = (maxPosition.y - minPosition.y) / totalSegments; // size of each segment.
	auto zSegments = (maxPosition.z - minPosition.z) / totalSegments; // size of each segment.

	float xPos = minPosition.x;
	float yPos = minPosition.y;
	float zPos = minPosition.z;


	for(auto x = 0; x < totalSegments; x++)
	{
		for(auto y = 0; y < totalSegments; y++)
		{
			for(auto z = 0; z < totalSegments; z++)
			{
				m_Positions.push_back(Vector3(xPos, yPos, zPos));

				int index = x * totalSegments * totalSegments + y * totalSegments + z;
				std::cout << "New min position for position number " << index << " is " <<
					m_Positions[index].x << ", " <<
					m_Positions[index].y << ", " <<
					m_Positions[index].z << ", " << std::endl;

				zPos += zSegments;
			}
			yPos += ySegments;
			zPos = minPosition.z;
		}
		xPos += xSegments;
		yPos = minPosition.y;
	}
}



void CollisionsManager::AddObjectToCollisionSegments(const Particle *)
{}



void CollisionsManager::ResetCollisionManager()
{
	// clear all segments of particles.
	for(auto segment : m_CollisionSegments)
	{
		segment.clear();
	}

	// clear active particle list.
	m_ActiveParticles.clear();
}
