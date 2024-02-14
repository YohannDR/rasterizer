#pragma once

#include "SudoMaths/vector4.h"

enum class BlendEquation
{
	ADD,
	SUBSTRACT,
	RESERVE_SUBSTRACT,
	MIN,
	MAX
};

enum class BlendOp
{
	ZERO,
	ONE,
	SRC_COLOR,
	ONE_MINUS_SRC_COLOR,
	DST_COLOR,
	ONE_MINUS_DST_COLOR,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	DST_ALPHA,
	ONE_MINUS_DST_ALPHA
};

class Blending
{
private:
	BlendOp m_LeftOp;
	BlendOp m_RightOp;
	BlendEquation m_Equation;

	Vector4 ComputeOperator(const BlendOp operation, const Vector4& dstColor, const Vector4& scrColor);
	Vector4 ComputeEquation(const BlendEquation equation, const Vector4& left, const Vector4& right);

public:
	bool Enabled;

	Blending();

	void SetBlending(const BlendOp leftOp, const BlendOp rightOp, const BlendEquation equation);

	/// <summary>
	/// Applies the blending operation
	/// </summary>
	/// <param name="dstColor">Previous color of the color buffer</param>
	/// <param name="scrColor">New color that should be applied</param>
	/// <returns>The blend between dstColor and scrColor</returns>
	Vector4 ComputeBlending(const Vector4& dstColor, const Vector4& scrColor);
};
