/*



*/
#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera()  = default;
	~Camera() = default;

	void SetPosition(DirectX::XMFLOAT3);
	void SetRotation(DirectX::XMFLOAT3);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMMATRIX GetViewMatrix();

	void Render();


private:
	DirectX::XMFLOAT3 m_position = {0.f, 0.f, 0.f};
	DirectX::XMFLOAT3 m_rotation = {0.f, 0.f, 0.f};
	DirectX::XMMATRIX m_viewMat;
};