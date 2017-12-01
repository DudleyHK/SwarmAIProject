/*



*/
#pragma once
#include <fstream>
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Particle.h"


class SwarmManager
{
public:
	SwarmManager() = default;
	~SwarmManager() = default;

	const bool Init(ID3D11Device*, ID3D11DeviceContext*, HWND, int instanceCount);
	void Update();
	const DirectX::XMMATRIX& GetWorldAt(const int index) const;
	DirectX::XMMATRIX* GetWorldMatrices();
	Particle* SwarmManager::GetParticleAt(const int index) const;


private:
	const bool InitShader(ID3D11Device*, ID3D11DeviceContext*, HWND);
	const bool CreateStructuredBuffer(ID3D11Device*);
	const bool CompileComputeShader(ID3D11Device*, WCHAR*, HWND);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void SetGlobalBestDistance();
	void UpdatePhysics();
	void CalculateParticleCollisions(const int index);
	void CalculateParticlePhysics(const int index);
	void UpdateWorldMatrix(const Particle*, const int index);

	const DirectX::XMFLOAT3 GetRandomPosition();
	const float CalculateDistance(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 CalculateDirection(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b);
	const DirectX::XMFLOAT3 NormaliseFloat3(const DirectX::XMFLOAT3 a);
	const DirectX::XMFLOAT3 ComputeForce(const DirectX::XMFLOAT3 dir, const float mass);


	struct ParticleConstantBuffer
	{
		DirectX::XMFLOAT3 m_globalBestPosition;
		float m_globalBestDistance;
	};

	struct ParticleStruct
	{
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_velocity;
		DirectX::XMFLOAT3 m_acceleration;
	};


	ID3D11Buffer* m_pConstantBuffer = nullptr;

	// Structured Buffers
	ID3D11Buffer*                       m_pParticles = nullptr;
	ID3D11ShaderResourceView*           m_pParticlesSRV = nullptr;
	ID3D11UnorderedAccessView*          m_pParticlesUAV = nullptr;

	ID3D11ComputeShader* m_pParticleComputeShader = nullptr;

	std::vector<ParticleStruct*> m_ParticleStructList;
	std::vector<std::unique_ptr<Particle>> m_Particles;
	std::vector<std::unique_ptr<DirectX::XMMATRIX>> m_WorldMatrices;

	const DirectX::XMFLOAT3 GOAL_POSITION = {0.f, 0.f, 0.f};

	int m_instanceCount;
	float m_deltaTime = 0.01f;
	float m_globalBestDistance = 0.f;
	DirectX::XMFLOAT3 m_globalBestPosition = {0.f, 0.f, 0.f};
};