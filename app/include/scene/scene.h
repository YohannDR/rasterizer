#pragma once

#include <vector>

#include "renderer/Vertex.h"
#include "renderer/renderer.h"

#include "engine/gameobject.h"

class Scene
{
private:
    std::vector<Vertex> m_Vertices;

    std::vector<GameObject> m_GameObjects;

    bool m_PeekFramebuffer;
    bool m_HasDrawn;

    void Ui_Controls(Renderer& renderer);
    void Ui_GameObjects(Renderer& renderer);
    void Ui_Framebuffer(Renderer& renderer);
    void Ui_Lights(Renderer& renderer);

public:
    Scene(Renderer& renderer);
    ~Scene();
    void Update(const float deltaTime, Renderer& renderer);

    void SetImGuiContext(struct ImGuiContext* context);
    void ShowImGuiControls(Renderer& renderer);
};
