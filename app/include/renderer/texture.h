#pragma once

#include <stdint.h>
#include <vector>
#include "SudoMaths/vector4.h"
#include "SudoMaths/vector2.h"

enum class TexFiltering
{
	NEAREST,
	LINEAR
};

class Texture
{
private:
	std::vector<Vector4> m_Data;
	int32_t m_Width;
	int32_t m_Height;

	TexFiltering m_Filtering;

	Vector4 ApplyFiltering(const Vector2 ntc, const Vector2 texCoords) const;

public:
	Texture();
	Texture(const char* const fileName);
	Texture(const uint8_t* const data, const int32_t width, const int32_t height, const int32_t nbrChannels);
	~Texture();

	void Load(const uint8_t* const data, const int32_t width, const int32_t height, const int32_t nbrChannels);

	void SetFiltering(const TexFiltering filtering);

	Vector4 SampleTexel(const Vector2 ntc) const;
};

