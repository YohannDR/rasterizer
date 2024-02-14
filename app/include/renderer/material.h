#pragma once

#include "SudoMaths/vector4.h"

class Material
{
public:
	Vector4 Ambient;
	Vector4 Diffuse;
	Vector4 Specular;

	float Shininess;

	Material();
	Material(const Vector4& ambient, const Vector4& diffuse, const Vector4& specular, const float shininess);
};

