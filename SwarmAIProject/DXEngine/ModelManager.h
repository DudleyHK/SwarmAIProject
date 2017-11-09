/*



*/
#pragma once
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>


class ModelManager
{
public:
	ModelManager() = default;
	~ModelManager() = default;

	const bool Init(ID3D11Device*, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	bool LoadModel(char*);
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

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	int m_vertexCount;
	int m_indexCount;

	std::vector<std::unique_ptr<ModelType>> m_pModelType;

};