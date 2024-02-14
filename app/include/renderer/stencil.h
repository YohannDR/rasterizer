#pragma once

#include <stdint.h>
#include <cmath>

enum class StencilOp
{
	WRITE,
	DISCARD
};

class Stencil
{
private:
	bool m_Enabled;

	StencilOp m_Operation;

	uint32_t m_Width;
	uint32_t m_Height;

public:
	float_t* StencilBuffer;

	Stencil(const float width, const float height);
	~Stencil();

	void SetOperation(const StencilOp operation);
	bool ApplyOperation(const uint32_t x, const uint32_t y);

	void SetEnable(const bool enabled);
};
