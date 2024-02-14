#include "renderer/material.h"

Material::Material()
	: Ambient(1.f), Diffuse(1.f), Specular(1.f), Shininess(1.f)
{
}

Material::Material(const Vector4& ambient, const Vector4& diffuse, const Vector4& specular, const float shininess)
	: Ambient(ambient), Diffuse(diffuse), Specular(specular), Shininess(shininess)
{
}

