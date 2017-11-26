/*



*/
#pragma once
#include <memory>
#include <vector>

#include "Particle.h"


class SwarmManager
{
public:
	SwarmManager() = default;
	~SwarmManager() = default;

	const bool Init(int instanceCount);
	void Update();
	const DirectX::XMMATRIX& GetWorldAt(const int index) const;
	DirectX::XMMATRIX* GetWorldMatrices();





private:
	void SetGlobalBestDistance();
	void CalculateParticlePhysics();
	void UpdateWorldMatrix(const Particle*, const int index);

	const DirectX::XMFLOAT3 GetRandomPosition();
	const float CalculateDistance(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 CalculateDirection(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 NormaliseFloat3(const DirectX::XMFLOAT3 a);
	const DirectX::XMFLOAT3 ComputeForce(const DirectX::XMFLOAT3 dir, const float mass);



	std::vector<std::unique_ptr<Particle>> m_Particles;
	std::vector<std::unique_ptr<DirectX::XMMATRIX>> m_WorldMatrices;

	const DirectX::XMFLOAT3 GOAL_POSITION = {0.f, 0.f, 0.f};

	int m_instanceCount;
	float m_deltaTime = 0.01f;
	float m_globalBestDistance = 0.f;
	DirectX::XMFLOAT3 m_globalBestPosition = {0.f, 0.f, 0.f};
};