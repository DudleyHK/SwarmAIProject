/*
*/
#include <fstream>

#include "Model.h"
#include "Utilities.h"



const bool Model::Init(ID3D11Device* device, char* modelFilename)
{
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

void Model::Shutdown()
{
	ShutdownBuffers();
	ReleaseModel();
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

bool Model::LoadModel(char* filename)
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

void Model::ReleaseModel()
{
	m_pModelType.clear();
}

const bool Model::InitBuffers(ID3D11Device* device)
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

	m_instanceCount = 100000;

	
	InstanceType* instances = new InstanceType[m_instanceCount];
	if(!instances)
	{
		return false;
	}


	auto x = 0.f;
	auto y = 0.f;
	auto z = 0.f;
	for(auto i = 0; i < m_instanceCount; i++)
	{
		z += 1.f;

		instances[i].position = DirectX::XMFLOAT3(x, y, z);
	}




	D3D11_BUFFER_DESC instanceBufferDesc = {0};
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_pInstanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	SafeDeleteArray(vertices);
	SafeDeleteArray(instances);

	return true;
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_pVertexBuffer;
	bufferPointers[1] = m_pInstanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int Model::GetVertexCount()
{
	return m_vertexCount;
}

int Model::GetInstanceCount()
{
	return m_instanceCount;
}


void Model::ShutdownBuffers()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pInstanceBuffer);
}
