/*
*/
#pragma once
#include <fstream>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>



class ColourShaderManager
{
public:
	ColourShaderManager() = default;
	~ColourShaderManager() = default;

	bool Init(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int vertexCount, int instanceCount, DirectX::XMMATRIX& worldMat, DirectX::XMMATRIX& viewMat, DirectX::XMMATRIX& projMat);


private:
	bool InitShader(ID3D11Device*, HWND, WCHAR* vsFilePath, WCHAR* psFilePath);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX& worldMat, DirectX::XMMATRIX& viewMat, DirectX::XMMATRIX& projMat);
	void RenderShader(ID3D11DeviceContext*, int vertexCount, int instanceCount);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	std::unique_ptr<MatrixBufferType> m_pConstantBuffer = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader*  m_pPixelShader = nullptr;
	ID3D11InputLayout*  m_pLayout = nullptr;
	ID3D11Buffer*       m_pMatrixBuffer = nullptr;
};