#include "pch.h"
#include "Camera.h"

Camera::Camera(const FVector3& pos, float FOV, float aspectRatio, uint32_t screenheight, uint32_t screenwidth, const float nearVal, const float farVal)
	:m_AspectRatio(aspectRatio)
	, m_FOV(FOV)
	, m_Height(screenheight)
	, m_Width(screenwidth)
	, m_NearValue(nearVal)
	, m_FarValue(farVal)
	, m_startPos(pos)

{
	m_ONB = FMatrix4::Identity();
	AddTranslation(pos);

	FMatrix4 ProjectionMatrix = FMatrix4();

	ProjectionMatrix(0, 0) = 1.f / (m_AspectRatio * m_FOV);
	ProjectionMatrix(1, 1) = 1.f / (m_FOV);
	ProjectionMatrix(2, 2) = farVal/ ( farVal - nearVal);
	ProjectionMatrix(2, 3) = (-(farVal * nearVal)) / (farVal- nearVal);
	ProjectionMatrix(3, 2) = 1.f;
	m_ProjectionMatrix = ProjectionMatrix;
}

Camera::~Camera()
{
}

void Camera::AddTranslation(const FVector3& translation)
{
	

	Elite::FMatrix3 rotation = Elite::MakeRotationZYX(m_rotation.x, m_rotation.y, m_rotation.z);

	FVector3 tempTranslation = rotation * translation;
	m_position += tempTranslation;
	m_ONB(0, 3) = m_position.x;
	m_ONB(1, 3) = m_position.y;
	m_ONB(2, 3) = m_position.z;
}



void Camera::AddRotation(FVector3 movement)
{
	
	m_rotation += FVector3(-movement.x, -movement.y, movement.z);
	Elite::FMatrix4 rotationMatrix = Elite::FMatrix4(Elite::MakeRotationZYX(m_rotation.x,m_rotation.y,m_rotation.z),m_position);
	m_ONB = rotationMatrix;

}

void Camera::ResetCamera()
{
	m_ONB = FMatrix4::Identity();
	m_position = m_startPos;
	AddTranslation(FVector3(0.f,0.f,0.f));
}

const FPoint3 Camera::GetPosition() const
{
	return FPoint3(m_ONB(0, 3), m_ONB(1, 3), m_ONB(2, 3));
}

const float Camera::GetFOV() const
{
	return m_FOV;
}

const float Camera::GetAspectRatio() const
{
	return m_AspectRatio;
}

const float Camera::GetNearValue()
{
	return m_NearValue;
}

const float Camera::GetFarValue()
{
	return m_FarValue;
}

const FMatrix4 Camera::getViewMatrix(bool isLeft) 
{
	int leftOrRight = isLeft ? -1 : 1;
	FMatrix4 ViewMatrix = m_ONB;
	ViewMatrix(0, 0) *= leftOrRight;
	ViewMatrix(1, 0) *= leftOrRight;
	ViewMatrix(2, 0) *= leftOrRight;
	ViewMatrix(0, 1) *= leftOrRight;
	ViewMatrix(1, 1) *= leftOrRight;
	ViewMatrix(2, 1) *= leftOrRight;


	return ViewMatrix;
}

const FMatrix4 Camera::getProjectionMatrix(bool isleft) const
{
	int leftOrRight = isleft ? -1 : 1;

	FMatrix4 SwitchMatrix = FMatrix4();
	SwitchMatrix(0, 0) = 1.f / (m_AspectRatio * m_FOV);
	SwitchMatrix(1, 1) = 1.f / (m_FOV);
	SwitchMatrix(2, 2) = m_FarValue / ((m_FarValue - m_NearValue) * leftOrRight);

	SwitchMatrix(2, 3) =  ((m_FarValue * m_NearValue) / (m_NearValue - m_FarValue)) * leftOrRight;
	SwitchMatrix(3, 2) = leftOrRight;



	return SwitchMatrix;
}

const FVector3 Camera::GetForward() const
{
	return FVector3(m_ONB(0, 2), m_ONB(1, 2), m_ONB(2, 2));
}



