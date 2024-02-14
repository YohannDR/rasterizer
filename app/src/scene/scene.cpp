#include "scene/scene.h"
#include "renderer/material.h"
#include "ImGui/imgui.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <chrono>
#include <iostream>

Scene::Scene(Renderer& renderer)
{
    m_PeekFramebuffer = false;
    m_HasDrawn = false;

    size_t vkRoom = renderer.AddTexture("assets/viking_room.jpg");

    {
        m_Vertices.clear();
        {
            m_Vertices.push_back(Vertex(
                Vector3(-0.5f, -0.5f, 0.0f),
                Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Vector2(0.0f, 0.0f))
            );

            m_Vertices.push_back(Vertex(
                Vector3(0.5f, -0.5f, 0.0f),
                Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Vector2(1.0f, 0.0f))
            );

            m_Vertices.push_back(Vertex(
                Vector3(0.0f, 0.5f, 0.0f),
                Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                Vector3(0.0f, 0.0f, 0.0f),
                Vector2(0.5f, 1.0f))
            );
        }
    }

    {
        m_GameObjects.clear();

        m_GameObjects.push_back(GameObject(
            Vector3(0.0f, 0.0f, 0.0f),
            Vector3(M_PI / 2.0f, 0.0f, 0.0f),
            Vector3(1.5f),
            "assets/viking_room.obj",
            Material(
                Vector4(1.0f, 0.0f, 0.0f, 1.0f),
                Vector4(1.0f),
                Vector4(1.0f),
                32.f
            ),
            vkRoom)
        );

        m_GameObjects.push_back(GameObject(
            Vector3(2.0f, 0.0f, 0.0f),
            Vector3(M_PI / 2.0f, 0.0f, 0.0f),
            Vector3(1.5f),
            "assets/viking_room.obj",
            Material(
                Vector4(1.0f, 0.0f, 0.0f, 1.0f),
                Vector4(1.0f),
                Vector4(1.0f),
                1.0f
            ),
            vkRoom)
        );
    }
}

Scene::~Scene()
{
}

void Scene::Update(const float deltaTime, Renderer& renderer)
{
    ShowImGuiControls(renderer);

    if (!m_HasDrawn)
    {
        using std::chrono::high_resolution_clock;

        std::chrono::steady_clock::time_point t1 = high_resolution_clock::now();

        // m_HasDrawn = true;
        renderer.ClearBuffers();

        for (uint32_t i = 0; i < m_GameObjects.size(); i++)
        {
            const GameObject& go = m_GameObjects[i];
            if (go.Outlined)
                go.RenderOutlined(renderer);
            else
                go.Render(renderer);
        }

        std::chrono::steady_clock::time_point t2 = high_resolution_clock::now();

        std::chrono::duration<double, std::milli> ms_double = t2 - t1;
        // std::cout << "Rendering time : " << ms_double << std::endl;
    }

    renderer.ForwardToImgui();
}

void Scene::SetImGuiContext(struct ImGuiContext* context)
{
}

void Scene::ShowImGuiControls(Renderer& renderer)
{
    Ui_Controls(renderer);
    Ui_GameObjects(renderer);
    Ui_Framebuffer(renderer);
    Ui_Lights(renderer);
}

void Scene::Ui_Controls(Renderer& renderer)
{
    if (ImGui::Begin("Controls"))
    {
        ImGui::Text("FPS : %f", 1.f / ImGui::GetIO().DeltaTime);
        ImGui::Text("Nbr triangles rendered : %d", renderer.NbrTrianglesRendered);
        if (ImGui::Button("Re-render"))
            m_HasDrawn = false;

        ImGui::SliderFloat3("Camera position", &renderer.Camera.Position.x, -2.f, 10.f);
        ImGui::SliderFloat3("Camera center", &renderer.Camera.Center.x, -2.f, 2.f);
        ImGui::Checkbox("Backface culling", &renderer.EnableBackfaceCulling);

        ImGui::SliderAngle("FOV", &renderer.Camera.Fov, 10.f, 90.f);

        ImGui::ColorPicker4("Clear color", &renderer.GetClearColor().x);

    }
    ImGui::End();
}

void Scene::Ui_GameObjects(Renderer& renderer)
{
    if (ImGui::Begin("Gameobjects"))
    {
        for (uint32_t i = 0; i < m_GameObjects.size(); i++)
        {
            GameObject& go = m_GameObjects[i];

            ImGui::PushID(i);

            ImGui::Separator();
            ImGui::Text("ID : %d", i);

            ImGui::Checkbox("Hidden", &go.Hidden);
            ImGui::Checkbox("Outlined", &go.Outlined);

            ImGui::SliderFloat3("Position", &go.Position.x, -5.f, 5.f);
            ImGui::SliderAngle("Rotation X", &go.Rotation.x, -360.f, 360.f);
            ImGui::SliderAngle("Rotation Y", &go.Rotation.y, -360.f, 360.f);
            ImGui::SliderAngle("Rotation Z", &go.Rotation.z, -360.f, 360.f);
            ImGui::SliderFloat3("Scaling", &go.Scaling.x, -3.f, 3.f);

            ImGui::PopID();
        }
    }

    ImGui::End();
}

void Scene::Ui_Framebuffer(Renderer& renderer)
{
    if (ImGui::Begin("Framebuffer data"))
    {
        ImGui::Checkbox("View framebuffer", &m_PeekFramebuffer);

        if (!m_PeekFramebuffer)
        {
            ImGui::End();
            return;
        }

        const Vector2 size = renderer.GetSize();
        for (uint32_t y = 0; y < size.y; y++)
        {
            for (uint32_t x = 0; x < size.x; x++)
            {
                const Vector4 pixel = renderer.GetPixel(x, y);
                ImGui::Text("%f ; %f ; %f ; %f |", pixel.x, pixel.y, pixel.z, pixel.w);
                ImGui::SameLine();
            }

            ImGui::Text("");
        }
    }

    ImGui::End();
}

void Scene::Ui_Lights(Renderer& renderer)
{
    if (ImGui::Begin("Lights"))
    {
        for (uint32_t i = 0; i < renderer.m_Lights.size(); i++)
        {
            Light& light = renderer.m_Lights[i];

            ImGui::PushID(i);

            ImGui::Separator();
            ImGui::Text("ID : %d", i);

            ImGui::Checkbox("Enabled", &light.Enabled);
            if (!light.Enabled)
            {
                ImGui::PopID();
                continue;
            }

            ImGui::SliderFloat3("Position", &light.Position.x, 0, 1000.f);

            ImGui::ColorPicker4("Ambiant", &light.Ambient.x);
            ImGui::ColorPicker4("Diffuse", &light.Diffuse.x);
            ImGui::ColorPicker4("Specular", &light.Specular.x);
            ImGui::SliderFloat("Specular strength", &light.SpecularStrength, 0.01f, 10.f);

            ImGui::SliderFloat("Linear att.", &light.LinearAttenuation, 0.f, 1.f);
            ImGui::SliderFloat("Quad att.", &light.QuadraticAttenuation, 0.f, 1.f);
            ImGui::SliderFloat("Radius", &light.Radius, 0.f, 1000.f);

            ImGui::PopID();
        }
    }

    ImGui::End();
}
