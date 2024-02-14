#pragma once

#include <stdint.h>
#include <vector>

#include "renderer/vertex.h"
#include "renderer/camera.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "renderer/light.h"
#include "renderer/blending.h"
#include "renderer/stencil.h"
#include "engine/gameobject.h"

#include "SudoMaths/matrix4x4.h"
#include "SudoMaths/vector2.h"

#define ARR_2D_IDX(x, y) (m_Width * (y) + (x))

class Renderer
{
private:
    struct Viewport
    {
        int32_t x;
        int32_t y;
        uint32_t width;
        uint32_t height;

        Viewport() {}

        Viewport(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height)
            : x(_x), y(_y), width(_width), height(_height)
        {}
    };

    uint32_t m_Width;
    uint32_t m_Height;

    int32_t m_FramebufferScale;

    uint32_t m_TextureId;

    Vector4* m_ColorBuffer;
    float_t* m_DepthBuffer;

    Vector4 m_ClearColor;

    Matrix4x4 m_Projection;
    Matrix4x4 m_View;
    Matrix4x4 m_Model;
    Viewport m_Viewport;

    std::vector<Texture> m_Textures;
    int32_t m_CurrentTexture;

    bool m_StopTime;
    float m_Time;

    Blending m_Blending;
    Stencil m_Stencil;

    void CreateFramebuffer();
    void UpdateFramebuffer();
    void DestroyFramebuffer();

    void DrawTriangle(const Vector4& p1, const Vector4& p2, const Vector4& p3,
        const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vector3& normal);

    Vector4 ApplyTransformationPipeline(const Vertex& vertex);
    Vector4 ApplyLights(const Vector3& position, const Vector4& currColor, const Vector3& normal);

    Vector4 NdcToScreenCoords(const Vector4& ndc, const bool ignoreZ);

    Vector3 m_CameraScreenPosition;

public:
    Camera Camera;
    std::vector<Light> m_Lights;
    Material CurrentMaterial;

    bool EnableBackfaceCulling;

    uint32_t NbrTrianglesRendered;

    Renderer(uint32_t width, uint32_t height);
    ~Renderer();

    void SetProjectionMatrix(const Matrix4x4& projection);
    void SetViewMatrix(const Matrix4x4& view);
    void SetModelMatrix(const Matrix4x4& model);
    void SetViewport(const Vector2 position, const Vector2 size);

    Vector4& GetClearColor();
    void SetClearColor(const Vector4& color);

    Vector2 GetSize();
    float GetTime();

    bool SetPixel(const uint32_t x, const uint32_t y);
    bool SetPixel(const uint32_t x, const uint32_t y, const Vector4& color);
    Vector4 GetPixel(const uint32_t x, const uint32_t y);

    void ProcessVertices(const std::vector<Vertex>& vertices);

    void ClearBuffers();
    void ForwardToImgui();

    void BindTexture(int32_t id);
    int32_t AddTexture(const char* const fileName);
    int32_t AddTexture(const uint8_t* const data, const uint32_t width, const uint32_t height, const uint32_t nbrChannels);

    void SetLightState(const uint32_t lightId, const bool enabled);

    void SetBlendState(const bool enabled);
    void SetBlendState(const BlendOp leftOp, const BlendOp rightOp, const BlendEquation equation);

    void SetStencilState(const bool enabled);
    void SetStencilState(const StencilOp operation);
    void SetStencilState(const bool enabled, const StencilOp operation);
    
    friend class Camera;
    friend class GameObject;
};
