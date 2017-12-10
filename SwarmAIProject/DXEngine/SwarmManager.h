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

	const DirectX::XMMATRIX& GetWorldAt(const int index) const;
	DirectX::XMMATRIX* GetWorldMatrices();
	Particle* SwarmManager::GetParticleAt(const int index) const;
	void SetGoalPosition(const DirectX::XMFLOAT4);

	const bool Init(ID3D11Device*, ID3D11DeviceContext*, HWND, int instanceCount);
	void Update(ID3D11DeviceContext*);
	void Shutdown();


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

	const DirectX::XMFLOAT4 GetRandomPosition();
	const float CalculateDistance(const DirectX::XMFLOAT4 a, const DirectX::XMFLOAT4 b);
	const DirectX::XMFLOAT4 CalculateDirection(const DirectX::XMFLOAT4 a, const DirectX::XMFLOAT4 b);
	const DirectX::XMFLOAT4 NormaliseFloat3(const DirectX::XMFLOAT4 a);
	const DirectX::XMFLOAT4 ComputeForce(const DirectX::XMFLOAT4 dir, const float mass);

	__declspec(align(16))
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4 m_goalPosition;
		DirectX::XMFLOAT4 m_bestPosition;
		float m_instanceCount;
		float m_moveForce;
		float m_seperationForce;
		float m_gravityAcceleration;
		bool  m_computeShaderImplementation;
	};

	struct ParticleData
	{
		DirectX::XMFLOAT4 m_globalBestPosition;
		DirectX::XMFLOAT4 m_seperationForce;
		DirectX::XMFLOAT4 m_position;
		DirectX::XMFLOAT4 m_velocity;
		float  m_mass;
		float m_globalBestDistance;
	};

	struct CollisionData
	{
		float m_closestDist;
		float m_radius;
		int   m_closetID;
	};

	


	// Structured Buffers
	ID3D11Buffer*                       m_pParticleInputBuffer = nullptr;
	ID3D11Buffer*                       m_pCollisionInputBuffer = nullptr;
	ID3D11Buffer*                       m_pOutputBuffer = nullptr;
	ID3D11Buffer*                       m_pDebugOutputBuffer = nullptr;
	ID3D11Buffer*                       m_pConstantBuffer = nullptr;
	ID3D11ShaderResourceView*           m_pParticleDataInputSRV = nullptr;
	ID3D11ShaderResourceView*           m_pCollisionDataInputSRV = nullptr;
	ID3D11UnorderedAccessView*          m_pOutputUAV = nullptr;
	ID3D11ComputeShader* m_pParticleComputeShader = nullptr;

	ParticleData* m_ParticleData;
	CollisionData* m_CollisionData;


	std::vector<std::unique_ptr<Particle>> m_Particles;
	std::vector<std::unique_ptr<DirectX::XMMATRIX>> m_WorldMatrices;

	DirectX::XMFLOAT4 m_goalPosition = {0.f, 0.f, 0.f, 0.f};
	const float GRAVITY_ACCELERATION = -9.8f;

	int m_instanceCount;
	float m_deltaTime = 0.01f;
	float m_globalBestDistance = 0.f;
	DirectX::XMFLOAT4 m_globalBestPosition = {0.f, 0.f, 0.f, 0.f};

	bool m_computeShaderImplementation = false;
};