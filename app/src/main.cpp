#include <vector>
#include <iostream>

#include "renderer/renderer.h"
#include "scene/scene.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "StbImage/stb_image.h"

int32_t SetupGlfw()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwSetErrorCallback(
        [](int error, const char* const description)
        {
            std::cerr << "GLFW error " << error << ": " << description << std::endl;
        }
    );

    if (!glfwInit())
        return false;

    return true;
}

GLFWwindow* CreateWindow()
{
    return glfwCreateWindow(800, 600, "Rasterizer", nullptr, nullptr);
}

void SetupWindow(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);

    gladLoadGL();

    glfwSwapInterval(1); // Enable vsync
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void SetupImGui(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable viewports

    io.Fonts->AddFontDefault();

    // GL 3.0 + GLSL 130
    const char* const glslVersion = "#version 130";

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void StartImGuiFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UpdateGlFrameBuffer(GLFWwindow* window)
{
    int displayW, displayH;
    glfwGetFramebufferSize(window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);

    glClear(GL_COLOR_BUFFER_BIT);
}

void Render(GLFWwindow* window)
{
    ImGui::Render();
    UpdateGlFrameBuffer(window);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GLFWwindow* ctxBackup = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(ctxBackup);
}

int main(int argc, char** argv)
{
    if (!SetupGlfw())
        return 1;

    GLFWwindow* window = CreateWindow();
    if (window == nullptr)
        return 1;

    SetupWindow(window);

    SetupImGui(window);

    const uint32_t width = 800;
    const uint32_t height = 600;

    Renderer* const renderer = new Renderer(width, height);
    Scene* const scene = new Scene(*renderer);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        StartImGuiFrame();

        scene->Update(ImGui::GetIO().DeltaTime, *renderer);

        // Rendering
        Render(window);

        glfwSwapBuffers(window);
    }

    // scnUpdate(scene, 1.f / 60.f, renderer);

    // printPPM(colorBuffer, width, height);

    delete scene;
    delete renderer;

    return 0;
}
