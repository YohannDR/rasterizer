#pragma once

#include "SudoMaths/vector3.h"
#include "SudoMaths/vector4.h"

#include "renderer/material.h"

class Renderer;

class Light
{
private:
	float ComputeLinearAttenuation(const Vector3& fragPos) const;
	float ComputeQuadraticAttenuation(const Vector3& fragPos) const;
	float ComputeLinearQuadraticAttenuation(const Vector3& fragPos) const;

public:
	bool Enabled;
	Vector3 Position;

	Vector4 Ambient;
	Vector4 Diffuse;
	Vector4 Specular;
	float SpecularStrength;

	float LinearAttenuation;
	float QuadraticAttenuation;
	float Radius;

	Light(const Vector3& position);
	Light(const Vector3& position, const Vector4& ambient, const Vector4& diffuse,
		const Vector4& specular);
	Light(const Vector3& position, const Vector4& ambient, const Vector4& diffuse,
		const Vector4& specular, const float specularStrenght, const float linearAttenuation, const float quadAttenuation,
		const float radius);

	Vector4 ComputeDiffuse(const Vector3& lightDir, const Vector3& fragPos,
		const Vector3& normal, const Material& material) const;
	Vector4 ComputeSpecular(const Vector3& lightDir, const Vector3& fragPos,
		const Vector3& camPos, const Vector3& normal, const Material& material) const;

	float ComputeAttenuation(const Vector3& fragPos) const;
	void Render(Renderer& renderer) const;
};
