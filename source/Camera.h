#pragma once
using namespace Elite;
class Camera
{
public:
	Camera(const FVector3& pos, float FOV, float aspectRatio, uint32_t height, uint32_t width, const float nearval, const float farval);
	~Camera();

	void AddTranslation(const FVector3& translation);
	void AddRotation(FVector3 xyzrotation);
	void ResetCamera();
	const FPoint3 GetPosition() const;
	const float GetFOV() const;
	const float GetAspectRatio() const;
	const float GetNearValue();
	const float GetFarValue();
	const FMatrix4 getViewMatrix(bool isLeft);
	const FMatrix4 getProjectionMatrix(bool isleft) const;
	const FVector3 GetForward() const;

private:
	FMatrix4 m_ONB;
	FMatrix4 m_ProjectionMatrix;
	FVector3 m_position{};
	FVector3 m_startPos;

	FVector3 m_rotation{};

	float m_FOV;
	float m_AspectRatio;
	float m_NearValue;
	float m_FarValue;


	uint32_t m_Width;
	uint32_t m_Height;
};

