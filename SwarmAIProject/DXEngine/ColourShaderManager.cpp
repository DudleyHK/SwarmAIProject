/*



*/
#include "ColourShaderManager.h"
#include "Utilities.h"


bool ColourShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
	auto result = InitShader(device, hwnd, L"../DXEngine/ResourceFiles/DefaultColour.vs.hlsl", L"../DXEngine/ResourceFiles/DefaultColour.ps.hlsl");
	if(!result)
	{
		return false;
	}
	return true;
}

void ColourShaderManager::Shutdown()
{
	ShutdownShader();
}

bool ColourShaderManager::Render(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, DirectX::XMMATRIX& worldMat, DirectX::XMMATRIX& viewMat, DirectX::XMMATRIX& projMat)
{
	auto result = SetShaderParameters(deviceContext, worldMat, viewMat, projMat);
	if(!result)
	{
		return false;
	}
	RenderShader(deviceContext, vertexCount, instanceCount);
	return true;
}

bool ColourShaderManager::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilePath, WCHAR* psFilePath)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	auto result = D3DCompileFromFile(vsFilePath, NULL, NULL, "ColourVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
									 &vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilePath);
		}
		else
		{
			MessageBox(hwnd, vsFilePath, L"Missing Vertex Shader File", MB_OK);
		}
		return false;
	}

	result = D3DCompileFromFile(psFilePath, NULL, NULL, "ColourPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
								&pixelShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilePath);
		}
		else
		{
			MessageBox(hwnd, psFilePath, L"Missing Pixel Shader File", MB_OK);
		}
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
	if(FAILED(result))
	{
		return false;
	}


	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
	if(FAILED(result))
	{
		return false;
	}



	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[7];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Instance Matrix
	polygonLayout[3].SemanticName = "INSTANCE";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

	polygonLayout[4].SemanticName = "INSTANCE";
	polygonLayout[4].SemanticIndex = 1;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[4].InputSlot = 1;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[4].InstanceDataStepRate = 1;

	polygonLayout[5].SemanticName = "INSTANCE";
	polygonLayout[5].SemanticIndex = 2;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 1;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[5].InstanceDataStepRate = 1;

	polygonLayout[6].SemanticName = "INSTANCE";
	polygonLayout[6].SemanticIndex = 3;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[6].InputSlot = 1;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[6].InstanceDataStepRate = 1;


	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements,
									   vertexShaderBuffer->GetBufferPointer(),
									   vertexShaderBuffer->GetBufferSize(),
									   &m_pLayout);
	if(FAILED(result))
	{
		return false;
	}

	SafeRelease(vertexShaderBuffer);
	SafeRelease(pixelShaderBuffer);

	D3D11_BUFFER_DESC matrixBufferDesc = {0};
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;



	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	//result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	m_pConstantBuffer = std::make_unique<MatrixBufferType>();
	if(!m_pConstantBuffer) 
		return false;
	ZeroMemory(m_pConstantBuffer.get(), sizeof(MatrixBufferType));

	return true;
}


void ColourShaderManager::ShutdownShader()
{
	SafeRelease(m_pMatrixBuffer);
	SafeRelease(m_pLayout);
	SafeRelease(m_pPixelShader);
	SafeRelease(m_pVertexShader);
}

void ColourShaderManager::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFilePath)
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



bool ColourShaderManager::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX& worldMat, DirectX::XMMATRIX& viewMat, DirectX::XMMATRIX& projMat)
{
	// Transpose the matrices to prepare them for the shader.
	worldMat = DirectX::XMMatrixTranspose(worldMat);
	viewMat = DirectX::XMMatrixTranspose(viewMat);
	projMat = DirectX::XMMatrixTranspose(projMat);

	m_pConstantBuffer->world = worldMat;
	m_pConstantBuffer->view = viewMat;
	m_pConstantBuffer->projection = projMat;

	deviceContext->UpdateSubresource(m_pMatrixBuffer, 0, NULL, m_pConstantBuffer.get(), 0, 0);

	//// create a resource which can be manipulated at run time if desired.
	//D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
	//auto result = deviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if(FAILED(result))
	//{
	//	return false;
	//}
	//MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	//dataPtr->world = worldMat;
	//dataPtr->view = viewMat;
	//dataPtr->projection = projMat;
	//deviceContext->Unmap(m_pMatrixBuffer, 0);
	//unsigned int bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	return true;
}

void ColourShaderManager::RenderShader(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount)
{
	deviceContext->IASetInputLayout(m_pLayout);

	deviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);
}