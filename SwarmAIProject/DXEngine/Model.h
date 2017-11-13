/*



*/
#pragma once
#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>


class Model
{
public:
	Model() = default;
	~Model() = default;

	const bool Init(ID3D11Device*, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetInstanceCount();

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
		DirectX::XMFLOAT3 instancePosition;
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
	ID3D11Buffer* m_pInstanceBuffer = nullptr;

	int m_vertexCount = 0;
	int m_instanceCount = 0;

	std::vector<std::unique_ptr<ModelType>> m_pModelType;

};