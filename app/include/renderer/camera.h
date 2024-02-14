#pragma once

#include "SudoMaths/vector3.h"

class Renderer;

class Camera
{
private:
	Renderer& m_Renderer;

	void SendViewMatrix();
	void SendProjectionMatrix();

public:
	Vector3 Position;
	Vector3 Center;

	float Fov;
	float DepthNear;
	float DepthFar;

	Camera(Renderer& renderer);
	Camera(const Vector3& position, const Vector3& center, Renderer& renderer);
	Camera(const Vector3& position, const Vector3& center,
		const float fov, const float zDepthNear, const float zDepthFar, Renderer& renderer);

	void Update();
};
