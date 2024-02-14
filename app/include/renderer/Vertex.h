#pragma once

#include "SudoMaths/vector2.h"
#include "SudoMaths/vector3.h"
#include "SudoMaths/vector4.h"

class Vertex
{
public:
    Vector3 m_Position;
    Vector4 m_Color;
    Vector3 m_Normal;
    Vector2 m_Uvs;

    Vertex(const Vector3& position);
    Vertex(const Vector3& position, const Vector4& color);
    Vertex(const Vector3& position, const Vector4& color, const Vector3& normal, const Vector2 uv);

    void SetPosition(const Vector3& position);
    void SetColor(const Vector4& color);
    void SetNormal(const Vector3& normal);
    void SetUv(const Vector2 uv);

    void SetPositionAndNormal(const Vector3& position);
};

