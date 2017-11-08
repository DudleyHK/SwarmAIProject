/*



*/
#include "ModelManager.h"
#include "Utilities.h"



const bool ModelManager::Init(ID3D11Device* device)
{
	if(!InitBuffers(device))
	{
		return false;
	}
	return true;
}

void ModelManager::Shutdown()
{
	ShutdownBuffers();
}

void ModelManager::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelManager::GetIndexCount()
{
	return m_indexCount;
}

const bool ModelManager::InitBuffers(ID3D11Device* device)
{
	m_vertexCount = 3;
	m_indexCount = 3;

	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// fill the vertex and index arrays.
	vertices[0].position = DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].colour   = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].colour = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].colour = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

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

	D3D11_BUFFER_DESC indexBufferDesc = {0};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	return true;
}

void ModelManager::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void ModelManager::ShutdownBuffers()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}


