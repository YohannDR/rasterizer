#include "renderer/renderer.h"

#include <algorithm>
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "ImGui/imgui.h"
#include "glad/glad.h"

#define MAX_AMOUNT_OF_LIGHTS 8

void Renderer::CreateFramebuffer()
{
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height,
        0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateFramebuffer()
{
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height,
        0, GL_RGBA, GL_FLOAT, m_ColorBuffer);
}

void Renderer::DestroyFramebuffer()
{
    glDeleteTextures(1, &m_TextureId);
}


Renderer::Renderer(uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height), Camera(*this), m_Stencil(width, height)
{
    m_ColorBuffer = new Vector4[width * height];
    m_DepthBuffer = new float_t[width * height];

    m_FramebufferScale = 1;

    CreateFramebuffer();
    SetClearColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    SetViewport(
        Vector2(0.0f, 0.0f),
        Vector2(m_Width, m_Height)
    );

    for (uint32_t i = 0; i < MAX_AMOUNT_OF_LIGHTS; i++)
    {
        m_Lights.push_back(Light(
            Vector3(m_Width / 2.f, m_Height / 4.f, 1.5f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
            Vector4(1.0f, 1.0f, 1.0f, 1.0f),
            0.5f, 0.2f, 0.0f, 200.f
        ));
    }

    // SetLightState(0, true);
    EnableBackfaceCulling = false;
}

Renderer::~Renderer()
{
    DestroyFramebuffer();
    delete[] m_ColorBuffer;
    delete[] m_DepthBuffer;
}

void Renderer::SetProjectionMatrix(const Matrix4x4& projection)
{
    m_Projection = projection;
}

void Renderer::SetViewMatrix(const Matrix4x4& view)
{
    m_View = view;
}

void Renderer::SetModelMatrix(const Matrix4x4& model)
{
    m_Model = model;
}

void Renderer::SetViewport(const Vector2 position, const Vector2 size)
{
    m_Viewport = Viewport(position.x, position.y, size.x, size.y);
}

Vector4& Renderer::GetClearColor()
{
    return m_ClearColor;
}

void Renderer::SetClearColor(const Vector4& color)
{
    m_ClearColor = color;
}

Vector2 Renderer::GetSize()
{
    return Vector2(m_Width, m_Height);
}

float Renderer::GetTime()
{
    return m_Time;
}

bool Renderer::SetPixel(const uint32_t x, const uint32_t y)
{
    if (x < 0 || x >= m_Width)
        return false;

    if (y < 0 || y >= m_Height)
        return false;

    m_ColorBuffer[ARR_2D_IDX(x, y)] = Vector4(1);
    return true;
}

bool Renderer::SetPixel(const uint32_t x, const uint32_t y, const Vector4& color)
{
    /*if (x < 0 || x >= m_Width)
        return false;

    if (y < 0 || y >= m_Height)
        return false;*/

    m_ColorBuffer[ARR_2D_IDX(x, y)] = color;
    return true;
}

Vector4 Renderer::GetPixel(const uint32_t x, const uint32_t y)
{
    return m_ColorBuffer[ARR_2D_IDX(x, y)];
}

void Renderer::ClearBuffers()
{
    for (uint32_t y = 0; y < m_Height; y++)
    {
        for (uint32_t x = 0; x < m_Width; x++)
        {
            uint32_t offset = ARR_2D_IDX(x, y);

            m_ColorBuffer[offset] = m_ClearColor;
            m_DepthBuffer[offset] = INFINITY;
            m_Stencil.StencilBuffer[offset] = 0.f;
        }
    }
}

void Renderer::ForwardToImgui()
{
    ImGuiIO& io = ImGui::GetIO();
    
    UpdateFramebuffer();

    if (ImGui::Begin("Framebuffer", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Checkbox("Stop", &m_StopTime);
        ImGui::SliderInt("Framebuffer scale", &m_FramebufferScale, 1, 4);

        const ImVec2 size = ImVec2(m_FramebufferScale * m_Width, m_FramebufferScale * m_Height);
        ImGui::Image((ImTextureID)m_TextureId, size);
    }
    ImGui::End();
}

void Renderer::DrawTriangle(const Vector4& p1, const Vector4& p2, const Vector4& p3,
    const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vector3& normal)
{
    if (EnableBackfaceCulling)
    {
        const Vector3 p = (Vector3(p1.x, p1.y, p1.z) - m_CameraScreenPosition).NormalizeSafe();
        if (Vector3::DotProduct(p, normal) >= 0)
            return;
    }

    NbrTrianglesRendered++;

    // Get color of each vertex
    const Vector4& c1 = v1.m_Color;
    const Vector4& c2 = v2.m_Color;
    const Vector4& c3 = v3.m_Color;

    // Get 2D coords of each vertex
    const Vector2 points[3] = {
        Vector2(p1.x, p1.y),
        Vector2(p2.x, p2.y),
        Vector2(p3.x, p3.y)
    };

    // Get the bounding box of the triangle (min/max of both X and Y)
    // and clamp that bounding box to viewport, which discards the pixels outside of the viewport
    // effectively clipping the triangle
    float minX = std::min(p1.x, std::min(p2.x, p3.x));
    minX = std::clamp<float>(minX, m_Viewport.x, m_Viewport.x + m_Viewport.width);

    float maxX = std::max(p1.x, std::max(p2.x, p3.x));
    maxX = std::clamp<float>(maxX, m_Viewport.x, m_Viewport.x + m_Viewport.width);

    float minY = std::min(p1.y, std::min(p2.y, p3.y));
    minY = std::clamp<float>(minY, m_Viewport.y, m_Viewport.y + m_Viewport.height);

    float maxY = std::max(p1.y, std::max(p2.y, p3.y));
    maxY = std::clamp<float>(maxY, m_Viewport.y, m_Viewport.y + m_Viewport.height);

    for (uint32_t y = minY; y < maxY; y++)
    {
        for (uint32_t x = minX; x < maxX; x++)
        {
            // Convert the pixel to a Vector 3
            Vector3 p = Vector3(x, y, 0.0f);

            // Compute the barycentric coordinates of the point

            // Compute the denominator for the weight 1 and weight 2
            const float denWeight = (points[1].y - points[2].y) * (points[0].x - points[2].x) +
                (points[2].x - points[1].x) * (points[0].y - points[2].y);

            // Compute each weight
            // If any weight is negative, then that means the pixel is outside the triangle, thus abort

            const float w1 = ((points[1].y - points[2].y) * (p.x - points[2].x) +
                (points[2].x - points[1].x) * (p.y - points[2].y)) / denWeight;
            if (w1 < 0)
                continue;

            const float w2 = ((points[2].y - points[0].y) * (p.x - points[2].x) +
                (points[0].x - points[2].x) * (p.y - points[2].y)) / denWeight;
            if (w2 < 0)
                continue;

            const float w3 = 1 - w1 - w2;
            if (w3 < 0)
                continue;

            const float persp = p1.w * w1 + p2.w * w2 + p3.w * w3;
            const Vector3 perspective = Vector3(w1, w2, w3) * Vector3(p1.w, p2.w, p3.w) * (1.f / persp);

            // Compute the depth of the current pixel
            const float depth = p1.z * w1 + p2.z * w2 + p3.z * w3;
            p.z = depth;

            if (depth < m_DepthBuffer[ARR_2D_IDX(x, y)])
            {
                m_DepthBuffer[ARR_2D_IDX(x, y)] = depth;
            }
            else
            {
                continue;
            }

            // Interpolate the color based on the weight of each point of the triangle and its color
            // Since the sum of the weight is equal to 1, we can simply add and multiply
            Vector4 color = c1 * perspective.x + c2 * perspective.y + c3 * perspective.z;

            if (m_Stencil.ApplyOperation(x, y))
            {
                // The stencil signaled that the fragment should be discarded
                continue;
            }

            if (m_CurrentTexture != -1)
            {
                const Texture& tex = m_Textures[m_CurrentTexture];

                // Get uv of each vertex and weight them out
                const Vector2 uv1 = v1.m_Uvs;
                const Vector2 uv2 = v2.m_Uvs;
                const Vector2 uv3 = v3.m_Uvs;
                const Vector2 uvs = uv1 * perspective.x + uv2 * perspective.y + uv3 * perspective.z;

                // Sample the corresponding texel
                // and multiply it with the color so that combining may be achieved
                color *= tex.SampleTexel(uvs);
            }

            color = ApplyLights(p, color, normal);
            
            if (m_Blending.Enabled)
            {
                color = m_Blending.ComputeBlending(GetPixel(x, y), color);
            }

            // Apply the resulting color
            SetPixel(x, y, color);
        }
    }
}

Vector4 Renderer::ApplyLights(const Vector3& position, const Vector4& currColor, const Vector3& normal)
{
    // Start from the current color
    Vector4 newColor = currColor;
    for (uint32_t i = 0; i < MAX_AMOUNT_OF_LIGHTS; i++)
    {
        const Light& light = m_Lights[i];

        if (!light.Enabled)
            continue;

        const Vector3 lightPos = light.Position;

        // Direction between light and fragment
        const Vector3 lightDir = Vector3(position, lightPos).Normalize();
        
        // Get diffuse and specular
        const Vector4 ambient = light.Ambient * CurrentMaterial.Ambient;
        const Vector4 diffuse = light.ComputeDiffuse(lightDir, position, normal, CurrentMaterial);
        const Vector4 specular = light.ComputeSpecular(lightDir, position, m_CameraScreenPosition,
            normal, CurrentMaterial);

        // Get actual computed light
        const Vector4 computedLight = (ambient + diffuse + specular);

        // Apply it to the color
        newColor *= computedLight;
    }

    return newColor;
}

void Renderer::ProcessVertices(const std::vector<Vertex>& vertices)
{
    assert(vertices.size() % 3 == 0 && "Number of vertices wasn't a multiple of 3");

    if (!m_StopTime)
        m_Time += ImGui::GetIO().DeltaTime;

    for (size_t i = 0; i < MAX_AMOUNT_OF_LIGHTS; i++)
    {
        const Light& light = m_Lights[i];

        light.Render(*this);
    }

    Camera.Update();
   
    const Vector4 camPos = NdcToScreenCoords(Camera.Position, true);
    m_CameraScreenPosition = Vector3(camPos.x, camPos.y, camPos.z);

    std::vector<Vector4> transformed = std::vector<Vector4>(vertices.size());
    std::vector<Vector3> normals = std::vector<Vector3>(vertices.size() / 3);

    const Matrix3x3 rotation = Matrix3x3(
        m_Model.Row0.x, m_Model.Row0.y, m_Model.Row0.z,
        m_Model.Row1.x, m_Model.Row1.y, m_Model.Row1.z,
        m_Model.Row2.x, m_Model.Row2.y, m_Model.Row2.z
    );

    for (size_t i = 0; i < vertices.size(); i++)
    {
        const Vertex& vertex = vertices[i];

        if ((i % 3) == 0)
        {
            normals[i / 3] = rotation.Multiply(vertex.m_Normal).NormalizeSafe();
        }

        transformed[i] = ApplyTransformationPipeline(vertex);
    }

    NbrTrianglesRendered = 0;
    for (size_t i = 0; i < vertices.size() / 3; i++)
    {
        const size_t tri = i * 3;
        /*if (i == 1)
        {
            m_Textures[m_CurrentTexture].SetFiltering(TexFiltering::LINEAR);
        }
        else
        {
            m_Textures[m_CurrentTexture].SetFiltering(TexFiltering::NEAREST);
        }*/

        DrawTriangle(transformed[tri], transformed[tri + 1], transformed[tri + 2],
            vertices[tri], vertices[tri + 1], vertices[tri + 2], normals[i]);
    }
}

void Renderer::BindTexture(int32_t id)
{
    m_CurrentTexture = id;
}

int32_t Renderer::AddTexture(const char* const filenName)
{
    m_Textures.push_back(Texture(filenName));
    return m_Textures.size() - 1;
}

int32_t Renderer::AddTexture(const uint8_t* const data, const uint32_t width, const uint32_t height, const uint32_t nbrChannels)
{
    m_Textures.push_back(Texture(data, width, height, nbrChannels));
    return m_Textures.size() - 1;
}

void Renderer::SetLightState(const uint32_t lightId, const bool enabled)
{
    assert(lightId < MAX_AMOUNT_OF_LIGHTS && "Light ID out of bounds");
    
    m_Lights[lightId].Enabled = enabled;
}

void Renderer::SetBlendState(const bool enabled)
{
    m_Blending.Enabled = enabled;
}

void Renderer::SetBlendState(const BlendOp leftOp, const BlendOp rightOp, const BlendEquation equation)
{
    m_Blending.SetBlending(leftOp, rightOp, equation);
}

void Renderer::SetStencilState(const bool enabled)
{
    m_Stencil.SetEnable(enabled);
}

void Renderer::SetStencilState(const StencilOp operation)
{
    m_Stencil.SetOperation(operation);
}

void Renderer::SetStencilState(const bool enabled, const StencilOp operation)
{
    m_Stencil.SetEnable(enabled);
    m_Stencil.SetOperation(operation);
}


Vector4 Renderer::ApplyTransformationPipeline(const Vertex& vertex)
{
    const Vector3& position = vertex.m_Position;

    // Convert to homogenous coords
    Vector4 coords = Vector4(position, 1.0f);

    // Calculate MVP
    Matrix4x4 mvp = m_Projection;
    mvp.Multiply(m_View).Multiply(m_Model);

    // Apply MVP on coords
    coords = mvp.Multiply(coords);

    coords.w = 1.f / coords.w;
    // Calculate NDC
    Vector4 ndc = Vector4(Vector3(coords.x, coords.y, coords.z) * coords.w, coords.w);

    // Apply viewport
    Vector4 screenCoords = NdcToScreenCoords(ndc, false);

    return screenCoords;
}

Vector4 Renderer::NdcToScreenCoords(const Vector4& ndc, const bool ignoreZ)
{
    return Vector4(
        (m_Viewport.width / 2.0f) * (ndc.x + 1) + m_Viewport.x,
        (m_Viewport.height / 2.0f) * (ndc.y + 1) + m_Viewport.y,
        ignoreZ ? ndc.z : (.5f * ndc.z + .5f),
        ndc.w
    );
}
