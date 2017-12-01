/*



*/
#include <iostream>
#include <random>



#include "SwarmManager.h"
#include "Utilities.h"




const bool SwarmManager::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int instanceCount)
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
		m_Particles[i]->SetID(i);


		// Fill the PArticleStructure List
		m_ParticleStructList.push_back(new ParticleStruct());
		m_ParticleStructList[i]->m_position = DirectX::XMFLOAT3(randPosition.x, randPosition.y, randPosition.z);


		//std::cout << "Initial position of Particle " << i << " is " <<
		//	randPosition.x << ", " <<
		//	randPosition.y << ", " <<
		//	randPosition.z << ", " << std::endl;

	}


	// Set a random global best distance to get it started.
	m_globalBestDistance = CalculateDistance(m_Particles[0]->GetPosition().DXFloat3(), GOAL_POSITION);
	m_globalBestPosition = m_Particles[0]->GetPosition().DXFloat3();
	

	if(!InitShader(device, deviceContext, hwnd))
		return false;



	return true;
}

void SwarmManager::Update()
{
	SetGlobalBestDistance();
	UpdatePhysics();
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

Particle* SwarmManager::GetParticleAt(const int index) const
{
	return m_Particles[index].get();
}


const bool SwarmManager::InitShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	// Create the buffers
	if(!CreateStructuredBuffer(device)) 
		return false;


	// Set up dynamic constant Buffer
	D3D11_BUFFER_DESC matrixBufferDesc = {0};
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ParticleConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	auto result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_pConstantBuffer);
	if(FAILED(result))
	{
		return false;
	}

	if(!CompileComputeShader(device, L"../DXEngine/ResourceFiles/ParticlePhysicsShader.cs.hlsl", hwnd))
		return false;
	
	deviceContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	deviceContext->CSSetShaderResources(1, 1, &m_pParticlesSRV);
	deviceContext->CSSetShader(m_pParticleComputeShader, nullptr, 0);
	deviceContext->Dispatch(m_instanceCount / 256, 1, 1);

	return true;
}



const bool SwarmManager::CreateStructuredBuffer(ID3D11Device* device)
{
	auto result = true;

	// Create the Structured buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = sizeof(ParticleStruct) * m_instanceCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = sizeof(ParticleStruct);

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory(&bufferInitData, sizeof(bufferInitData));
	bufferInitData.pSysMem = m_ParticleStructList[0];

	// auto particleData = (m_ParticleStructList.data()) ? &bufferInitData : nullptr;
	result = device->CreateBuffer(&bufferDesc, &bufferInitData, &m_pParticles);
	if(FAILED(result)) return false;


	// create the shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = m_instanceCount;

	result = device->CreateShaderResourceView(*&m_pParticles, &srvDesc, &m_pParticlesSRV);
	if(FAILED(result)) return false;


	// Create unordered access view 
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.Buffer.NumElements = m_instanceCount;

	result = device->CreateUnorderedAccessView(*&m_pParticles, &uavDesc, &m_pParticlesUAV);
	if(FAILED(result)) return false;


	return true;
}


const bool SwarmManager::CompileComputeShader(ID3D11Device* device, WCHAR* csFilePath, HWND hwnd)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* computeShaderBuffer = nullptr;

	auto result = D3DCompileFromFile(csFilePath, NULL, NULL, "ParticlePhysicsShader", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
									 &computeShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, csFilePath);
		}
		else
		{
			MessageBox(hwnd, csFilePath, L"Missing Compute Shader File", MB_OK);
		}
		return false;
	}

	device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), nullptr, &m_pParticleComputeShader);
	if(FAILED(result)) return false;

	return true;
}



void SwarmManager::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilePath)
{
	std::ofstream fout;

	char* compileErrors = compileErrors = (char*)(errorMsg->GetBufferPointer());
	unsigned int bufferSize = bufferSize = errorMsg->GetBufferSize();
	fout.open("shader-error.txt");

	for(unsigned int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();
	SafeRelease(errorMsg);
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilePath, MB_OK);

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

void SwarmManager::UpdatePhysics()
{
	for(int i = 0; i < m_instanceCount; i++)
	{
		//CalculateParticleCollisions(i);
		CalculateParticlePhysics(i);
	}
}



void SwarmManager::CalculateParticleCollisions(const int index)
{
	auto particle = m_Particles[index].get();
	particle->ClearNeighbours();
	particle->SetSeperationForce(Vector3::Zero());

	// Find the closest neighbouring particle.
	int   closestID = -1;
	float closestDist = 10000.f;

	for(auto i = 0; i < m_Particles.size(); i++)
	{
		auto neighbour = m_Particles[i].get();
		if(neighbour->GetID() == particle->GetID()) continue;

		auto distance = CalculateDistance(particle->GetPosition().DXFloat3(), neighbour->GetPosition().DXFloat3());
		auto radius = particle->GetRadius();

		//std::cout << "Distance between particle and neighbour is " << distance << std::endl;
		//std::cout << "Radius squared is " << (radius * radius) << std::endl;
		
		if(distance < (radius * radius))
		{
			if(distance < closestDist)
			{
				closestDist = distance;
				closestID = i;
			}
		}
	}
	// A closest neighbour hasn't been identified
	if(closestID <= -1) return;

	// Push the particle away from the nearest neighbour.
	auto direction = CalculateDirection(particle->GetPosition().DXFloat3(), m_Particles[closestID]->GetPosition().DXFloat3());
	//auto normalisedDirection = NormaliseFloat3(direction);
	auto force = ComputeForce(direction, particle->GetMass());

	// auto oppositeForce = Particle::ConvertToVector3(force) * -1.f;

	// particle->SetSeperationForce(oppositeForce);
}



void SwarmManager::CalculateParticlePhysics(const int index)
{
	auto particle = m_Particles[index].get();

	auto direction = CalculateDirection(particle->GetPosition().DXFloat3(), m_globalBestPosition);
	auto normalisedDirection = NormaliseFloat3(direction);
	auto force = ComputeForce(normalisedDirection, particle->GetMass());

	particle->CalcLoads(force);
	particle->UpdateBodyEuler(m_deltaTime);

	UpdateWorldMatrix(particle, index);
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
	auto force = 50.f;

	auto x = (dir.x * force) * mass;
	auto y = (dir.y * force) * mass;
	auto z = (dir.z * force) * mass;


	return DirectX::XMFLOAT3(x, y, z);
}