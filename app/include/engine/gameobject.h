#pragma once

#include "renderer/material.h"
#include "renderer/vertex.h"

#include "SudoMaths/vector3.h"
#include "SudoMaths/matrix4x4.h"

#include <vector>

class Renderer;

class GameObject
{
private:
	std::vector<Vertex> m_Vertices;

public:
	Vector3 Position;
	Vector3 Rotation;
	Vector3 Scaling;

	Material ModelMaterial;
	size_t TextureId;

	bool Hidden;
	bool Outlined;

	GameObject();
	GameObject(const Vector3& position, const Vector3& rotation, const Vector3& scaling);
	GameObject(const Vector3& position, const Vector3& rotation, const Vector3& scaling,
		const char* const modelName, const Material& material, const size_t textureId);

	void LoadModel(const char* const name);

	void CalculateModelMatrix(Matrix4x4& model) const;

	void Render(Renderer& renderer) const;
	void RenderOutlined(Renderer& renderer) const;
};

