/*
*/
#pragma once
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

#include "Rigidbody.h"


class Model
{
public:
	Model() = default;
	~Model() = default;

	const bool Init(ID3D11Device*, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();	

	DirectX::XMMATRIX& GetWorldMatrix();
	void SetWorldMatrix(DirectX::XMMATRIX&);

	DirectX::XMFLOAT3 GetPosition();
	void SetPosition(DirectX::XMFLOAT3);

	DirectX::XMFLOAT3 GetBest();
	void SetBest(DirectX::XMFLOAT3);

	DirectX::XMFLOAT3 GetGlobalBest();
	void SetGlobalBest(DirectX::XMFLOAT3);


	bool LoadModel(char* filename);
	void ReleaseModel();




private:
	const bool InitBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoords;
		DirectX::XMFLOAT3 normals;
	};
	struct ModelType
	{
		float x;
		float y;
		float z;

		float tu;
		float tv;

		float nx;
		float ny;
		float nz;
	};
	struct InstanceType
	{
		DirectX::XMFLOAT3 position;
	};

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	int m_vertexCount;
	int m_indexCount;

	std::vector<std::unique_ptr<ModelType>> m_pModelType;

	DirectX::XMMATRIX m_worldMat;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_best;
	DirectX::XMFLOAT3 m_globalBest;

	std::unique_ptr<Rigidbody> m_pRigidbody = nullptr;


};