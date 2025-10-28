#include "TG/TGImGuiLayer.h"
#include <glad/glad.h>
#include "Base/App.h"
#include "Base/Window.h"


namespace tg
{
    TGImGuiLayer::TGImGuiLayer()
    {
    }

    TGImGuiLayer::TGImGuiLayer(const std::string& name)
    {
    }

    TGImGuiLayer::~TGImGuiLayer()
    {
    }

    void TGImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void TGImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        App& app = App::Get();
        auto [width, height] = app.GetWindow().GetSize();
        io.DisplaySize = ImVec2(float(width), float(width));

        //TODO: Render to swapchain... how do we handle this better?
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void TGImGuiLayer::OnAttach()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        {
            io.FontDefault = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\segoeui.ttf)", 18.0f);
            if (!io.FontDefault)
                io.FontDefault = io.Fonts->AddFontDefault();
        }

        ImGui::StyleColorsDark();
        SetDarkThemeV2Colors();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

        App& app = App::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

    void TGImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void TGImGuiLayer::OnImGuiRender()
    {
        ImGuiLayer::OnImGuiRender();
    }
}
