#include "renderer/renderer.h"
#include "renderer/camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera(Renderer& renderer)
	: m_Renderer(renderer)
{
	Position = Vector3(0.0f, -1.5f, 3.5f);
	Center = Vector3(0.0f);

	Fov = M_PI / 2.0f;
	DepthNear = 0.1f;
	DepthFar = 100.0f;

	SendViewMatrix();
	SendProjectionMatrix();
}

Camera::Camera(const Vector3& position, const Vector3& center, Renderer& renderer)
	: Position(position), Center(center), m_Renderer(renderer)
{
	Fov = M_PI / 2.0f;
	DepthNear = 0.1f;
	DepthFar = 100.0f;

	SendViewMatrix();
	SendProjectionMatrix();
}

Camera::Camera(const Vector3& position, const Vector3& center,
	const float fov, const float zDepthNear, const float zDepthFar, Renderer& renderer)
	: Position(position), Center(center), Fov(fov), DepthNear(zDepthNear), DepthFar(zDepthFar), m_Renderer(renderer)
{
	SendViewMatrix();
	SendProjectionMatrix();
}

void Camera::SendViewMatrix()
{
	Matrix4x4::View(
		Position,
		Center,
		Vector3(0.0f, 1.0f, 0.0f),
		m_Renderer.m_View
	);
}

void Camera::SendProjectionMatrix()
{
	const float aspectRatio = (float)m_Renderer.m_Width / m_Renderer.m_Height;

	Matrix4x4::Projection(
		Fov, aspectRatio, DepthNear, DepthFar, m_Renderer.m_Projection
	);
}

void Camera::Update()
{
	SendViewMatrix();
	SendProjectionMatrix();
}
