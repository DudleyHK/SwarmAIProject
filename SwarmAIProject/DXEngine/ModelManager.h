/*



*/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>


class ModelManager
{
public:
	ModelManager() = default;
	~ModelManager() = default;

	const bool Init(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();



private:
	const bool InitBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 colour;
	};

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	int m_vertexCount;
	int m_indexCount;

};