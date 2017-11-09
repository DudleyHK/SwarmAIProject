/*



*/
#include "Camera.h"



void Camera::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}



void Camera::SetRotation(DirectX::XMFLOAT3 rotation)
{
	m_rotation = rotation;
}



DirectX::XMFLOAT3 Camera::GetPosition()
{
	return m_position;
}



DirectX::XMFLOAT3 Camera::GetRotation()
{
	return m_rotation;
}



void Camera::Render()
{
	DirectX::XMFLOAT3 up = {0.f, 1.f, 0.f};
	DirectX::XMFLOAT3 lookAt = {0.f, 0.f, 1.f};
	DirectX::XMFLOAT3 position = m_position;

	float pitch = DirectX::XMConvertToRadians(m_rotation.x);
	float yaw   = DirectX::XMConvertToRadians(m_rotation.y);
	float roll  = DirectX::XMConvertToRadians(m_rotation.z);

	DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(yaw, pitch, roll);
	
	// Convert the float3 variables to vectors.
	DirectX::XMVECTOR lookAtVec = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&lookAt), rotationMat);
	DirectX::XMVECTOR upVec    = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&up), rotationMat);
	DirectX::XMVECTOR positionVec = DirectX::XMLoadFloat3(&position);

	lookAtVec = DirectX::XMVectorAdd(positionVec, lookAtVec);
	m_viewMat = DirectX::XMMatrixLookAtLH(positionVec, lookAtVec, upVec);

}



void Camera::GetViewMatrix(DirectX::XMMATRIX& viewMat)
{
	viewMat = m_viewMat;
}
