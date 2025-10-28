#include "RuntimeLayer.h"
#include <imgui.h>
#include <glfw/glfw3.h>

#include "Base/Window.h"
#include "TG/TGManager.h"

RuntimeLayer::RuntimeLayer()
{
    TG(LayerLog,Info,"RuntimeLayer created")
}

RuntimeLayer::~RuntimeLayer()
{
}

void RuntimeLayer::OnAttach()
{
    Layer::OnAttach();
    tg::TGManager::Get().Init();
}

void RuntimeLayer::OnDetach()
{
    tg::TGManager::Get().Shutdown();
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate(tg::TimeStep ts)
{
    Layer::OnUpdate(ts);
}

void RuntimeLayer::OnImGuiRender()
{

   // Setup docking
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    static bool dockspaceOpen = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);
    
    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
               // tg::App::Get().GetWindow().SetShouldClose(true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Demo Window", nullptr, &mShowDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                // Show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    
    ImGui::End();
    
    // Render Telegram Manager (this includes the main panel and all chat windows)
    tg::TGManager::Get().Render();
    
    // Optional: Show ImGui demo window for reference
    if (mShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&mShowDemoWindow);
    }
}
