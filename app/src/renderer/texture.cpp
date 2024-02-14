#include "renderer/texture.h"
#include "StbImage/stb_image.h"

#include <iostream>
#include <cmath>
#include <algorithm>

Texture::Texture()
	//: m_Data(nullptr), m_Width(0), m_Height(0)
	: m_Width(0), m_Height(0), m_Filtering(TexFiltering::NEAREST)
{
}

Texture::Texture(const char* const fileName)
{
	int32_t nbrChannels;
	const uint8_t* const data = stbi_load(fileName, &m_Width, &m_Height, &nbrChannels, 0);
	Load(data, m_Width, m_Height, nbrChannels);
}

Texture::Texture(const uint8_t* const data, const int32_t width, const int32_t height, const int32_t nbrChannels)
{
	Load(data, width, height, nbrChannels);
}

Texture::~Texture()
{
	/*if (m_Data != nullptr)
		delete[] m_Data;*/
}

void Texture::Load(const uint8_t* const data, const int32_t width, const int32_t height, const int32_t nbrChannels)
{
	m_Filtering = TexFiltering::LINEAR;
	m_Width = width;
	m_Height = height;

	// m_Data = new Vector3[width * height];

	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			const uint32_t offset = y * m_Width + x;
			const uint32_t texOffset = (y * m_Width + x) * nbrChannels;

			const float a = nbrChannels == 4 ? data[texOffset + 3] : 255.f;
			Vector4 pixel = Vector4(
				data[texOffset + 0],
				data[texOffset + 1],
				data[texOffset + 2],
				a) / 255.f;
			m_Data.push_back(pixel);
		}
	}
}

void Texture::SetFiltering(const TexFiltering filtering)
{
	m_Filtering = filtering;
}

Vector4 Texture::SampleTexel(const Vector2 ntc) const
{
	const Vector2 size = Vector2(m_Width, m_Height);
	Vector2 texCoords = ntc * size;

	texCoords.x = std::clamp(texCoords.x, 0.0f, (float)(m_Width - 1));
	texCoords.y = std::clamp(texCoords.y, 0.0f, (float)(m_Height - 1));

	return ApplyFiltering(ntc, texCoords);
}

Vector4 Texture::ApplyFiltering(const Vector2 ntc, const Vector2 texCoords) const
{
	Vector4 result;
	const Vector2 flooredCoords = Vector2(std::floor(texCoords.x), std::floor(texCoords.y));

	switch (m_Filtering)
	{
		case TexFiltering::NEAREST:
		{
			const uint32_t offset = flooredCoords.y * m_Width + flooredCoords.x;

			result = m_Data[offset];
			break;
		}

		case TexFiltering::LINEAR:
		{
			const uint32_t offset = flooredCoords.y * m_Width + flooredCoords.x;

			result = m_Data[offset];
			break;
		}
	}

	return result;
}
