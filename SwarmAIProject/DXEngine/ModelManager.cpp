/*
*/
#include <fstream>
#include <string>

#include "ModelManager.h"
#include "Utilities.h"





const bool ModelManager::Init(ID3D11Device* device, char* modelFilename)
{
	m_Instances.resize(m_instanceCount);


	if(!LoadModel(modelFilename))
	{
		return false;
	}

	if(!InitBuffers(device))
	{
		return false;
	}
	return true;
}

void ModelManager::Shutdown()
{
	ShutdownBuffers();
	ReleaseModel();
}

void ModelManager::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

void ModelManager::SetWorldAt(int index, DirectX::XMMATRIX& worldMat)
{
	m_Instances[index].worldMatrix = worldMat;
}

void ModelManager::SetInstancesWorld(DirectX::XMMATRIX* worldMatrices)
{
	for(auto i = 0; i < m_Instances.size(); i++)
	{
		auto worldMat = worldMatrices[i];
		m_Instances[i].worldMatrix = worldMat;
	}
}



bool ModelManager::LoadModel(char* filename)
{
	char input;
	std::ifstream fileIn;

	fileIn.open(filename);
	if(fileIn.fail())
		return false;


	fileIn.get(input);
	while(input != ':')
	{
		fileIn.get(input);
	}

	fileIn >> m_vertexCount;
	m_pModelType.reserve(m_vertexCount);

	fileIn.get(input);
	while(input != ':')
	{
		fileIn.get(input);
	}
	fileIn.get(input);
	fileIn.get(input);

	for(auto i = 0; i < m_vertexCount; i++)
	{
		m_pModelType.push_back(std::make_unique<ModelType>());
		fileIn >> m_pModelType[i]->x >> m_pModelType[i]->y >> m_pModelType[i]->z;
		fileIn >> m_pModelType[i]->tu >> m_pModelType[i]->tv;
		fileIn >> m_pModelType[i]->nx >> m_pModelType[i]->ny >> m_pModelType[i]->nz;
	}

	fileIn.close();
	return true;
}

void ModelManager::ReleaseModel()
{
	m_pModelType.clear();
}

const bool ModelManager::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	for(auto i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position  = {m_pModelType[i]->x,  m_pModelType[i]->y, m_pModelType[i]->z};
		vertices[i].texCoords = {m_pModelType[i]->tu, m_pModelType[i]->tv};
		vertices[i].normals   = {m_pModelType[i]->nx, m_pModelType[i]->ny, m_pModelType[i]->nz};
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {0};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	auto result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}


	D3D11_BUFFER_DESC instanceBufferDesc = {0};
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&instanceBufferDesc, 0, &m_pInstanceBuffer);
	if(FAILED(result))
	{
		return false;
	}
	SafeDeleteArray(vertices);

	return true;
}


void ModelManager::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	///////////////////////////////// MAP ////////////////////////////////
	D3D11_MAPPED_SUBRESOURCE mappedData;
	deviceContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	InstanceType* dataView = reinterpret_cast<InstanceType*>(mappedData.pData);

	for(auto i = 0; i < m_Instances.size(); i++)
	{
		auto worldMat = m_Instances[i];
		dataView[i] = worldMat;
	}
	deviceContext->Unmap(m_pInstanceBuffer, 0);

	//////////////////// RENDER BUFFERS //////////////////////////////////////
	unsigned int strides[2] = {sizeof(VertexType), sizeof(InstanceType)};
	unsigned int offsets[2] = {0, 0};
	ID3D11Buffer* bufferPointers[2] = {m_pVertexBuffer, m_pInstanceBuffer};

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int ModelManager::GetVertexCount()
{
	return m_vertexCount;
}

int ModelManager::GetInstanceCount()
{
	return m_instanceCount;
}


void ModelManager::ShutdownBuffers()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pInstanceBuffer);
}