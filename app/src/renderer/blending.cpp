#include "renderer/blending.h"

Blending::Blending()
{
	Enabled = false;

	m_LeftOp = BlendOp::SRC_ALPHA;
	m_RightOp = BlendOp::ONE_MINUS_SRC_ALPHA;
	m_Equation = BlendEquation::ADD;
}

void Blending::SetBlending(const BlendOp leftOp, const BlendOp rightOp, const BlendEquation equation)
{
	m_LeftOp = leftOp;
	m_RightOp = rightOp;
	m_Equation = equation;
}

Vector4 Blending::ComputeBlending(const Vector4& dstColor, const Vector4& srcColor)
{
	const Vector4 left = ComputeOperator(m_LeftOp, dstColor, srcColor) * srcColor;
	const Vector4 right = ComputeOperator(m_RightOp, dstColor, srcColor) * dstColor;

	const Vector4 result = ComputeEquation(m_Equation, left, right);

	return result;
}

Vector4 Blending::ComputeOperator(const BlendOp operation, const Vector4& dstColor, const Vector4& srcColor)
{
	Vector4 result;
	switch (operation)
	{ 
		case BlendOp::ZERO:
			result = Vector4(0.f);
			break;

		case BlendOp::ONE:
			result = Vector4(1.f);
			break;

		case BlendOp::SRC_COLOR:
			result = srcColor;
			break;

		case BlendOp::ONE_MINUS_SRC_COLOR:
			result = Vector4(1.f) - srcColor;
			break;

		case BlendOp::DST_COLOR:
			result = dstColor;
			break;

		case BlendOp::ONE_MINUS_DST_COLOR:
			result = Vector4(1.f) - dstColor;
			break;

		case BlendOp::SRC_ALPHA:
			result = Vector4(srcColor.w);
			break;

		case BlendOp::ONE_MINUS_SRC_ALPHA:
			result = Vector4(1.f - srcColor.w);
			break;

		case BlendOp::DST_ALPHA:
			result = Vector4(dstColor.w);
			break;

		case BlendOp::ONE_MINUS_DST_ALPHA:
			result = Vector4(1.f - dstColor.w);
			break;
	}

	return result;
}

Vector4 Blending::ComputeEquation(const BlendEquation equation, const Vector4& left, const Vector4& right)
{
	Vector4 result;

	switch (equation)
	{
		case BlendEquation::ADD:
			result = left + right;
			break;

		case BlendEquation::SUBSTRACT:
			result = left - right;
			break;

		case BlendEquation::RESERVE_SUBSTRACT:
			result = right - left;
			break;

		case BlendEquation::MIN:
			result = Vector4(
				std::min(left.x, right.x),
				std::min(left.y, right.y),
				std::min(left.z, right.z),
				std::min(left.w, right.w)
			);
			break;

		case BlendEquation::MAX:
			result = Vector4(
				std::max(left.x, right.x),
				std::max(left.y, right.y),
				std::max(left.z, right.z),
				std::max(left.w, right.w)
			);
			break;
	}

	return result;
}
