/*



*/
#pragma once
#include <vector>

#include "Vector3.h"

class Particle;

class CollisionsManager
{
public:
	CollisionsManager()  = default;
	~CollisionsManager() = default;
	
	const bool Init(const int instanceCount);
	void Update();

	void AddParticle(Particle*);





private:

	void PartitionWorld();
	void AddObjectToCollisionSegments(const Particle*);
	void ResetCollisionManager();


	Vector3 maxPosition = Vector3(100.f, 100.f, 100.f);
	Vector3 minPosition = Vector3(-100.f, -100.f, -100.f);
	int totalSegments = 3;

	std::vector<Particle*> m_ActiveParticles;
	std::vector<std::vector<Particle*>> m_CollisionSegments;
	std::vector<Vector3> m_Positions;

};