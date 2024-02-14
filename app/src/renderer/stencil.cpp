#include "renderer/stencil.h"
#include "renderer/renderer.h"

Stencil::Stencil(const float width, const float height)
	: m_Width(width), m_Height(height)
{
	m_Enabled = false;
	m_Operation = StencilOp::WRITE;

	StencilBuffer = new float_t[width * height];
}

Stencil::~Stencil()
{
	delete[] StencilBuffer;
}

void Stencil::SetOperation(const StencilOp operation)
{
	m_Operation = operation;
}

bool Stencil::ApplyOperation(const uint32_t x, const uint32_t y)
{
	if (!m_Enabled)
		return false;

	bool discard = false;
	uint32_t offset = y * m_Width + x;

	switch (m_Operation)
	{
		case StencilOp::WRITE:
			StencilBuffer[offset] = 1.f;
			break;

		case StencilOp::DISCARD:
			if (StencilBuffer[offset] != 0.f)
				discard = true;
	}

	return discard;
}

void Stencil::SetEnable(const bool enabled)
{
	m_Enabled = enabled;
}
