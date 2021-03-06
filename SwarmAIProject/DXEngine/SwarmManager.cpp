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
	m_ParticleData  = new ParticleData[m_instanceCount];
	m_CollisionData = new CollisionData[m_instanceCount];

	// give each particle a random veloctiy and position;
	for(auto i = 0; i < m_instanceCount; i++)
	{
		// Set CPU side particle Data.
		auto randPosition = GetRandomPosition();

		auto worldMat = DirectX::XMMatrixTranslation(randPosition.x, randPosition.y, randPosition.z);
		m_WorldMatrices.push_back(std::make_unique<DirectX::XMMATRIX>(worldMat));

		m_Particles.push_back(std::make_unique<Particle>());
		m_Particles[i]->SetPosition(Particle::ConvertToVector3(randPosition));
		m_Particles[i]->SetID(i);



		// Set buffer/ GPU side data.
		m_ParticleData[i].m_position = DirectX::XMFLOAT4(randPosition.x, randPosition.y, randPosition.z, 0.f);
		m_ParticleData[i].m_velocity = {0.f, 0.f, 0.f, 0.f};
		m_ParticleData[i].m_mass = 1.f;
		m_ParticleData[i].m_seperationForce = {1.f, 1.f, 1.f, 0.f};

		m_CollisionData[i].m_closestDist = 0.f;
		m_CollisionData[i].m_closetID = 0;
		m_CollisionData[i].m_radius = m_Particles[i]->GetRadius();
	}



	// Set a random global best distance to get it started.
	m_globalBestDistance = CalculateDistance(m_Particles[0]->GetPosition().DXFloat4(), m_goalPosition);
	m_globalBestPosition = m_Particles[0]->GetPosition().DXFloat4();
	

	if(!InitShader(device, deviceContext, hwnd))
		return false;



	return true;
}

void SwarmManager::Update(ID3D11DeviceContext* deviceContext)
{
	if(!m_computeShaderImplementation)
	{
		SetGlobalBestDistance();
		UpdatePhysics();
	}

	deviceContext->CSSetShaderResources(0, 1, &m_pParticleDataInputSRV);
	deviceContext->CSSetShaderResources(1, 1, &m_pCollisionDataInputSRV);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &m_pOutputUAV, NULL);
	deviceContext->CSSetShader(m_pParticleComputeShader, NULL, 0);
	deviceContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);


	deviceContext->Dispatch(m_instanceCount/ 256, 1, 1);

	deviceContext->CSSetShader(NULL, NULL, 0);
	ID3D11UnorderedAccessView* ppUAViewNULL[1] = {NULL};
	deviceContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

	ID3D11ShaderResourceView* ppSRVNULL[2] = {NULL, NULL};
	deviceContext->CSSetShaderResources(0, 2, ppSRVNULL);






	std::cout << std::endl << std::endl; 

	ParticleData* particleData = new ParticleData[m_instanceCount];

	//deviceContext->Flush();
	////////// DEBUG OUTPUT //////
	//deviceContext->CopyResource(m_pDebugOutputBuffer, m_pOutputBuffer);
//D3D11_MAPPED_SUBRESOURCE mappedData;
//deviceContext->Map(m_pOutputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
//
//auto dataView = reinterpret_cast<CollisionData*>(mappedData.pData);
//
//for(int i = 0; i < m_instanceCount; i++)
//{
//	auto collisionData = dataView[i];
//	
//	std::cout << "[GPU] Particle id (" << i << ") closest neighbour is particle " << collisionData.m_closetID <<
//		" at a distance of " << collisionData.m_closestDist << std::endl;
//
//	//std::cout << "GPU particle " << i << " whose position is (" << 
		//	particle.m_position.x << ", " <<
		//	particle.m_position.y << ", " <<
		//	particle.m_position.z << ")" << std::endl;
		//
		//std::cout << "GPU particle " << i << " direction is (" << dataView[i].m_forces.x << ", " <<
		//	dataView[i].m_forces.y << ", " <<
		//	dataView[i].m_forces.z << ")" << std::endl;
		//
		//std::cout << "GPU particle " << i << " whose velocity is (" << dataView[i].m_velocity.x << ", " <<
		//	dataView[i].m_velocity.y << ", " <<
		//	dataView[i].m_velocity.z << ")" << std::endl;
		//
		//std::cout << "GPU particle " << i << " forces is (" << particle.m_forces.x << ", " <<
		//	particle.m_forces.y << ", " <<
		//	particle.m_forces.z << ")" << std::endl;
		//
		//std::cout << "GPU particle " << i << " gravity is (" << dataView[i].m_gravity.x << ", " <<
		//	dataView[i].m_gravity.y << ", " <<
		//	dataView[i].m_gravity.z << ")" << std::endl;
		//
		//std::cout << "GPU particle " << i << " whose speed is (" << dataView[i].m_speed << ")" << std::endl;
		//
		//auto particleDist = CalculateDistance(particle.m_position, m_goalPosition);
		//if(particleDist < m_globalBestDistance)
		//{
		//	m_globalBestDistance = particleDist;
		//	m_globalBestPosition = particle.m_position;
		//}
		//
		//auto freshWorld = DirectX::XMMatrixIdentity();
		//auto translation = DirectX::XMMatrixTranslation(particle.m_position.x, particle.m_position.y, particle.m_position.z);
		//freshWorld = DirectX::XMMatrixTranspose(translation);
		//*m_WorldMatrices[i].get() = freshWorld;
//}
//deviceContext->Unmap(m_pOutputBuffer, 0);

	deviceContext->CopyResource(m_pDebugOutputBuffer, m_pOutputBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	deviceContext->Map(m_pDebugOutputBuffer, 0, D3D11_MAP_READ, 0, &mappedData);

	auto dataView = reinterpret_cast<ParticleData*>(mappedData.pData);

	for(int i = 0; i < m_instanceCount; i++)
	{
		auto particle = dataView[i];

		if(!m_computeShaderImplementation)
		{
			particleData[i].m_globalBestDistance = particle.m_globalBestDistance;
			particleData[i].m_globalBestPosition = particle.m_globalBestPosition;
			particleData[i].m_position = m_Particles[i]->GetPosition().DXFloat4();
			particleData[i].m_seperationForce = particle.m_seperationForce;
			particleData[i].m_mass = 1.f;

			m_Particles[i]->SetSeperationForce(particle.m_seperationForce);
			m_Particles[i]->SetPosition(particle.m_position);
		}
		else
		{
			particleData[i].m_position = particle.m_position;
			particleData[i].m_velocity = particle.m_velocity;
			particleData[i].m_seperationForce = particle.m_seperationForce;
			particleData[i].m_mass = 1.f;
		}



		auto particleDist = CalculateDistance(particle.m_position, m_goalPosition);
		if(particleDist < m_globalBestDistance)
		{
			m_globalBestDistance = particleDist;
			m_globalBestPosition = particle.m_position;
		}
	}
	deviceContext->Unmap(m_pDebugOutputBuffer, 0);
	deviceContext->UpdateSubresource(m_pParticleInputBuffer, 0, nullptr, particleData, 0, 0);
	

	if(m_computeShaderImplementation)
	{
		// Before destroying the particleData structure use it to update best positions and
		//   world matrix.
		for(auto i = 0; i < m_instanceCount; i++)
		{
			auto particle = particleData[i];

			auto particleDist = CalculateDistance(particle.m_position, m_goalPosition);
			if(particleDist < m_globalBestDistance)
			{
				m_globalBestDistance = particleDist;
				m_globalBestPosition = particle.m_position;
			}

			auto freshWorld = DirectX::XMMatrixIdentity();
			auto translation = DirectX::XMMatrixTranslation(particle.m_position.x, particle.m_position.y, particle.m_position.z);
			freshWorld = DirectX::XMMatrixTranspose(translation);

			*m_WorldMatrices[i].get() = freshWorld;
		}
	}
	
	SafeDelete(particleData);
	ConstantBuffer pData = {};


	pData.m_goalPosition = m_goalPosition;
	pData.m_bestPosition = m_globalBestPosition;
	pData.m_instanceCount = m_instanceCount;
	pData.m_moveForce = 50.f;
	pData.m_seperationForce = 25.f;
	pData.m_gravityAcceleration = GRAVITY_ACCELERATION;
	pData.m_computeShaderImplementation = m_computeShaderImplementation;


	deviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &pData, 0, 0);
	deviceContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);


	/*



	////////// UPDATE CONSTANT BUFFER //////
	deviceContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	ParticleConstantBuffer pData = {};

	pData.m_bestPosition = DirectX::XMFLOAT4(m_globalBestPosition.x, m_globalBestPosition.y, m_globalBestPosition.z, 0);
	pData.m_goalPosition = DirectX::XMFLOAT4(m_goalPosition.x, m_goalPosition.y, m_goalPosition.z, 0);
	pData.m_basicForce = 50.f;
	pData.m_gravityAcceleration = GRAVITY_ACCELERATION;

	deviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr , &pData, 0, 0);
   



   ////////// SWARM DATA UPDATE //////
  //D3D11_MAPPED_SUBRESOURCE mappedSwarmData;
  //deviceContext->Map(m_pInputSwarmBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSwarmData);
  //
  //auto dataSwarmView = reinterpret_cast<SwarmData*>(mappedSwarmData.pData);
  //for(int i = 0; i < m_instanceCount; i++)
  //{
	//   //std::cout << "Looking at particle " << i << " whose position is (" << dataView[i].m_position.x << ", " <<
	   //	dataView[i].m_position.y << ", " << 
	   //	dataView[i].m_position.z << ")" << std::endl;
	   //
	   //std::cout << "Looking at particle " << i << " whose velocity is (" << dataView[i].m_velocity.x << ", " <<
	   //	dataView[i].m_velocity.y << ", " <<
	   //	dataView[i].m_velocity.z << ")" << std::endl;
	   //
	   //std::cout << "Looking at particle " << i << " whose forces is (" << dataView[i].m_forces.x << ", " <<
	   //	dataView[i].m_forces.y << ", " <<
	   //	dataView[i].m_forces.z << ")" << std::endl;
  //
	//   std::cout << "Particle " << i << " basic force is .... " <<
	//	   dataSwarmView[i].basicForce << std::endl;
  //
	//   dataSwarmView->m_bestPosition = m_globalBestPosition;
	//   dataSwarmView->m_goalPosition = m_goalPosition;
	//   dataSwarmView->basicForce = 50.f;
	//   dataSwarmView->gravityAcceleration = GRAVITY_ACCELERATION;
  //}
  //deviceContext->Unmap(m_pInputSwarmBuffer, 0);
  //
  //deviceContext->CSSetShaderResources(1, 1, &m_pInputSwarmSRV);
  */
}

void SwarmManager::Shutdown()
{
	SafeDeleteArray(m_ParticleData);
	SafeDeleteArray(m_CollisionData);
	SafeRelease(m_pParticleInputBuffer);
	SafeRelease(m_pCollisionInputBuffer);
	SafeRelease(m_pOutputBuffer);
	SafeRelease(m_pConstantBuffer);
	SafeRelease(m_pDebugOutputBuffer);
	SafeRelease(m_pParticleDataInputSRV);
	SafeRelease(m_pCollisionDataInputSRV);
	SafeRelease(m_pOutputUAV);
	SafeRelease(m_pParticleComputeShader);
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

void SwarmManager::SetGoalPosition(const DirectX::XMFLOAT4 goalPosition)
{
	m_goalPosition = goalPosition;
	m_globalBestDistance = CalculateDistance(m_Particles[0]->GetPosition().DXFloat4(), m_goalPosition);
	m_globalBestPosition = m_Particles[0]->GetPosition().DXFloat4();
}


const bool SwarmManager::InitShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	// Create the buffers
	if(!CreateStructuredBuffer(device)) 
		return false;

	if(!CompileComputeShader(device, L"../DXEngine/ResourceFiles/ParticlePhysicsShader.cs.hlsl", hwnd))
		return false;


	return true;
}



const bool SwarmManager::CreateStructuredBuffer(ID3D11Device* device)
{
	auto result = S_OK;

	// Create a input buffer for particle data
	D3D11_BUFFER_DESC inputParticleData;
	ZeroMemory(&inputParticleData, sizeof(inputParticleData));
	inputParticleData.Usage = D3D11_USAGE_DEFAULT;
	inputParticleData.ByteWidth = sizeof(ParticleData) * m_instanceCount;
	inputParticleData.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputParticleData.CPUAccessFlags = 0;
	inputParticleData.StructureByteStride = sizeof(ParticleData);
	inputParticleData.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory(&bufferInitData, sizeof(bufferInitData));
	bufferInitData.pSysMem = m_ParticleData;

	result = device->CreateBuffer(&inputParticleData, &bufferInitData, &m_pParticleInputBuffer);
	if(FAILED(result)) return false;



	// Create a input buffer for collision data
	D3D11_BUFFER_DESC inputCollisionData;
	ZeroMemory(&inputCollisionData, sizeof(inputCollisionData));
	inputCollisionData.Usage = D3D11_USAGE_DEFAULT;
	inputCollisionData.ByteWidth = sizeof(CollisionData) * m_instanceCount;
	inputCollisionData.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputCollisionData.CPUAccessFlags = 0;
	inputCollisionData.StructureByteStride = sizeof(CollisionData);
	inputCollisionData.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	D3D11_SUBRESOURCE_DATA collisionBufferInitData;
	ZeroMemory(&collisionBufferInitData, sizeof(collisionBufferInitData));
	collisionBufferInitData.pSysMem = m_CollisionData;

	result = device->CreateBuffer(&inputCollisionData, &collisionBufferInitData, &m_pCollisionInputBuffer);
	if(FAILED(result)) return false;



	D3D11_BUFFER_DESC outputDesc;
	ZeroMemory(&outputDesc, sizeof(outputDesc));
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(ParticleData) * m_instanceCount;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(ParticleData);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	result = device->CreateBuffer(&outputDesc, 0, &m_pOutputBuffer);
	if(FAILED(result)) return false;


	D3D11_BUFFER_DESC debugOutputDesc;
	ZeroMemory(&debugOutputDesc, sizeof(debugOutputDesc));
	debugOutputDesc.Usage = D3D11_USAGE_STAGING;
	debugOutputDesc.BindFlags = 0;
	debugOutputDesc.ByteWidth = sizeof(ParticleData) * m_instanceCount;
	debugOutputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	debugOutputDesc.StructureByteStride = sizeof(ParticleData);
	debugOutputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	result = device->CreateBuffer(&debugOutputDesc, 0, &m_pDebugOutputBuffer);
	if(FAILED(result)) return false;


	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = m_instanceCount;

	result = device->CreateShaderResourceView(m_pParticleInputBuffer, &srvDesc, &m_pParticleDataInputSRV);
	if(FAILED(result)) return false;

	result = device->CreateShaderResourceView(m_pCollisionInputBuffer, &srvDesc, &m_pCollisionDataInputSRV);
	if(FAILED(result)) return false;





	// Create unordered access view 
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = m_instanceCount;

	result = device->CreateUnorderedAccessView(m_pOutputBuffer, &uavDesc, &m_pOutputUAV);
	if(FAILED(result)) return false;


	// Set up constant Buffer
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = sizeof(ConstantBuffer);

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_pConstantBuffer);
	if(FAILED(result))
		return false;


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

	SafeRelease(computeShaderBuffer);

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
//	std::cout << "Current goal position is " << "(" << m_goalPosition.x << ", " << m_goalPosition.y << ", " << m_goalPosition.z << std::endl;

	for(auto i = 0; i < m_instanceCount; i++)
	{
		auto particle = m_Particles[i].get();
		auto particleDist = CalculateDistance(particle->GetPosition().DXFloat4(), m_goalPosition);

	//	std::cout << "Particle distance of particle " << i << " is " <<
	//		particleDist;
	//	std::cout << " ||| Global Best Distance is " << m_globalBestDistance;
	//	std::cout << " ||| Global Best Position is " << " (" << 
	//		m_globalBestPosition.x << ", " <<
	//		m_globalBestPosition.y << ", " <<
	//		m_globalBestPosition.z << ") " << std::endl;
	//
		if(particleDist < m_globalBestDistance)
		{
			m_globalBestDistance = particleDist;
			m_globalBestPosition = particle->GetPosition().DXFloat4();
		 
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
	std::cout << std::endl << std::endl;

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
	particle->SetSeperationForce({0.f, 0.f, 0.f, 0.f});

	// Find the closest neighbouring particle.
	int   closestID = -1;
	float closestDist = 10000.f;

	for(auto i = 0; i < m_Particles.size(); i++)
	{
		auto neighbour = m_Particles[i].get();
		if(neighbour->GetID() == particle->GetID()) continue;

		auto distance = CalculateDistance(particle->GetPosition().DXFloat4(), neighbour->GetPosition().DXFloat4());
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
	auto direction = CalculateDirection(particle->GetPosition().DXFloat4(), m_Particles[closestID]->GetPosition().DXFloat4());
	auto normalisedDirection = NormaliseFloat3(direction);
	auto force = ComputeForce(normalisedDirection, particle->GetMass());

	// auto oppositeForce = Particle::ConvertToVector3(force) * -1.f;

	// particle->SetSeperationForce(oppositeForce);
}



void SwarmManager::CalculateParticlePhysics(const int index)
{
	auto particle = m_Particles[index].get();

	auto direction = CalculateDirection(particle->GetPosition().DXFloat4(), m_globalBestPosition);
	auto normalisedDirection = NormaliseFloat3(direction);
	auto force = ComputeForce(normalisedDirection, particle->GetMass());

	//std::cout << "CPU particle " << " gravity is (" << particle->m_gravity.x << ", " <<
	//	particle->m_gravity.y << ", " <<
	//	particle->m_gravity.z << ")" << std::endl;

	particle->CalcLoads(force);
	particle->UpdateBodyEuler(m_deltaTime);

	UpdateWorldMatrix(particle, index);
}



void SwarmManager::UpdateWorldMatrix(const Particle* particle, const int index)
{
	auto pPos = particle->GetPosition();
	auto freshWorld = DirectX::XMMatrixIdentity();

	//std::cout << "CPU position of particle " << index << " is " <<
	//	pPos.x << ", " <<
	//	pPos.y << ", " <<
	//	pPos.z << ", " << std::endl;
	
	//std::cout << "CPU velocity of particle " << index << " is " <<
	//	particle->GetVelocity().x << ", " <<
	//	particle->GetVelocity().y << ", " <<
	//	particle->GetVelocity().z << ", " << std::endl;

	 //std::cout << "CPU force of particle "  << index << " is " <<
	 //	particle->m_forces.x << ", " <<
	 //	particle->m_forces.y << ", " <<
	 //	particle->m_forces.z << ") " << std::endl;

	auto translation = DirectX::XMMatrixTranslation(pPos.x, pPos.y, pPos.z);
	freshWorld = DirectX::XMMatrixTranspose(translation);

	*m_WorldMatrices[index].get() = freshWorld;
}



const DirectX::XMFLOAT4 SwarmManager::GetRandomPosition()
{
	std::random_device randDevice;
	std::mt19937 generate(randDevice());  
	std::uniform_real_distribution<> distribute(static_cast<double>(-100), static_cast<double>(100)); // both inclusive


	auto x = static_cast<float>(distribute(generate));
	auto y = static_cast<float>(distribute(generate));
	auto z = static_cast<float>(distribute(generate));

	return {x, y, z, 0.f};

}



const float SwarmManager::CalculateDistance(const DirectX::XMFLOAT4 a, const DirectX::XMFLOAT4 b)
{
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	float z = (b.z - a.z);
	float w = (b.w - a.w);

	return (x * x) + (y * y) + (z * z) + (w * w); // remove sqrt because its performance heavy.
}



const DirectX::XMFLOAT4 SwarmManager::CalculateDirection(const DirectX::XMFLOAT4 a, const DirectX::XMFLOAT4 b)
{
	auto x = b.x - a.x;
	auto y = b.y - a.y;
	auto z = b.z - a.z;

	//std::cout << "CPU diretion of particle "  << " is (" <<
	//	x << ", " <<
	//	y << ", " <<
	//	z << ") " << std::endl;

	return DirectX::XMFLOAT4(x, y, z, 0.f);
}


const DirectX::XMFLOAT4 SwarmManager::NormaliseFloat3(const DirectX::XMFLOAT4 a)
{
	auto aVec = DirectX::XMLoadFloat4(&a);
	auto aVecNormalised = DirectX::XMVector3Normalize(aVec);

	DirectX::XMFLOAT4 aNormalised;
	DirectX::XMStoreFloat4(&aNormalised, aVecNormalised);

	return aNormalised;
}


const DirectX::XMFLOAT4 SwarmManager::ComputeForce(const DirectX::XMFLOAT4 dir, const float mass)
{
	auto force = 300.f;

	auto x = (dir.x * force) * mass;
	auto y = (dir.y * force) * mass;
	auto z = (dir.z * force) * mass;


	return DirectX::XMFLOAT4(x, y, z, 0.f);
}