/*



*/
#pragma once
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>


class SwarmManager
{
private:
	struct Particle
	{
		Particle(DirectX::XMFLOAT3 position = {0.f, 0.f, 0.f}, DirectX::XMFLOAT3 velocity = {0.f, 0.f, 0.f}, float mass = 1.f)
		{
			m_position = position;
			m_velocity = velocity;
			m_mass = mass;
		}

		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_velocity;
		float m_mass;
	};

public:
	SwarmManager() = default;
	~SwarmManager() = default;

	const bool Init(int instanceCount);
	void Update(std::vector<DirectX::XMMATRIX>&& instanceMatrices);
	std::vector<DirectX::XMMATRIX> GetWorldMatrices();





private:
	void SetGlobalBestDistance();
	void CalculateParticlePhysics();
	void UpdateWorldMatrix(const Particle*, const int index);

	const DirectX::XMFLOAT3 GetRandomPosition();
	const float CalculateDistance(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 CalculateDirection(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 NormaliseFloat3(const DirectX::XMFLOAT3 a);
	const DirectX::XMFLOAT3 ComputeForce(const DirectX::XMFLOAT3 dir, const float mass);
	const DirectX::XMFLOAT3 CalculateAcceleration(const DirectX::XMFLOAT3 force, const float mass);



	std::vector<DirectX::XMMATRIX> m_instanceMatrices;
	std::vector<std::unique_ptr<Particle>> m_Particles;

	const DirectX::XMFLOAT3 GOAL_POSITION = {0.f, 0.f, 0.f};

	float m_globalBestDistance = 0.f;
	DirectX::XMFLOAT3 m_globalBestPosition = {0.f, 0.f, 0.f};
};