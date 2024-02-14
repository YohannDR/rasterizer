#include "renderer/light.h"
#include "renderer/renderer.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Light::Light(const Vector3& position)
	: Position(position)
{
	Ambient = Vector4(1.0f);
	Diffuse = Vector4(1.0f);
	Specular = Vector4(1.0f);
	SpecularStrength = .5f;

	LinearAttenuation = 1.0f;
	QuadraticAttenuation = 0.0f;
	Radius = INFINITY;

	Enabled = false;
}

Light::Light(const Vector3& position, const Vector4& ambient, const Vector4& diffuse,
	const Vector4& specular)
	: Position(position), Ambient(ambient), Diffuse(diffuse), Specular(specular)
{
	SpecularStrength = .5f;

	LinearAttenuation = 1.0f;
	QuadraticAttenuation = 0.0f;
	Radius = INFINITY;

	Enabled = false;
}

Light::Light(const Vector3& position, const Vector4& ambient, const Vector4& diffuse,
	const Vector4& specular, const float specularStrenght, const float linearAttenuation, const float quadAttenuation,
	const float radius)
	: Position(position), Ambient(ambient), Diffuse(diffuse), Specular(specular), SpecularStrength(specularStrenght),
	  LinearAttenuation(linearAttenuation), QuadraticAttenuation(quadAttenuation), Radius(radius)
{
	Enabled = false;
}

Vector4 Light::ComputeDiffuse(const Vector3& lightDir, const Vector3& fragPos, const Vector3& normal,
	const Material& material) const
{
	const float lightIntensity = ComputeAttenuation(fragPos);

	// Compute intensity using dot product
	const float diff = std::max(Vector3::DotProduct(normal, lightDir), 0.0f);

	const float intensity = diff * lightIntensity;
	const Vector4 diffuse = Diffuse * material.Diffuse * intensity;

	return diffuse;
}

Vector4 Light::ComputeSpecular(const Vector3& lightDir, const Vector3& fragPos,
	const Vector3& camPos, const Vector3& normal, const Material& material) const
{
	const float lightIntensity = ComputeAttenuation(fragPos);

	// Direction between camera and fragment
	const Vector3 viewDir = (camPos - fragPos).Normalize();

	// Reflect the light vector off of the surface
	const Vector3 reflectDir = Vector3::Reflect(-lightDir, normal);

	// Compute intensity using dot product
	const float specularIntensity = std::max(Vector3::DotProduct(viewDir, reflectDir), 0.0f);
	// Apply shininess
	const float specularPower = std::pow(specularIntensity, material.Shininess);

	const float intensity = specularPower * SpecularStrength * lightIntensity;
	const Vector4 specular = Specular * material.Specular * intensity;

	return specular;
}

// Formulas are based on this : https://docs.blender.org/manual/en/2.79/render/blender_render/lighting/lights/attenuation.html

float Light::ComputeAttenuation(const Vector3& fragPos) const
{
	if (QuadraticAttenuation <= 0.f)
		return ComputeLinearAttenuation(fragPos);

	if (LinearAttenuation <= 0.f)
		return ComputeQuadraticAttenuation(fragPos);

	if (LinearAttenuation <= 0.f && QuadraticAttenuation <= 0.f)
		return 1.f;

	return ComputeLinearQuadraticAttenuation(fragPos);
}

float Light::ComputeLinearAttenuation(const Vector3& fragPos) const
{
	const float distance = Vector3::Distance(fragPos, Position);

	return Radius / (Radius + LinearAttenuation * distance);
}

float Light::ComputeQuadraticAttenuation(const Vector3& fragPos) const
{
	const float distanceSq = Vector3::DistanceSquared(fragPos, Position);
	const float radiusSq = Radius * Radius;

	return radiusSq / (radiusSq + QuadraticAttenuation * distanceSq);
}

float Light::ComputeLinearQuadraticAttenuation(const Vector3& fragPos) const
{
	const float distanceSq = Vector3::DistanceSquared(fragPos, Position);
	const float radiusSq = Radius * Radius;

	const float linear = Radius / (Radius + LinearAttenuation * std::sqrt(distanceSq));
	const float quad = radiusSq / (radiusSq + QuadraticAttenuation * distanceSq);

	return linear * quad;
}

void Light::Render(Renderer& renderer) const
{
	if (!Enabled)
		return;

	Vector4 color = Ambient + Diffuse + Specular;
	const Vector2 size = renderer.GetSize();

	for (int32_t y = -5; y < 5; y++)
	{
		for (int32_t x = -5; x < 5; x++)
		{
			const float _x = std::clamp<float>(Position.x + x, 0, size.x - 1);
			const float _y = std::clamp<float>(Position.y + y, 0, size.y - 1);
			renderer.SetPixel(_x, _y, color);
		}
	}
}

