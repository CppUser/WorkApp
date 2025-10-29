#include "RuntimeLayer.h"
#include <imgui.h>
#include <glfw/glfw3.h>

#include "Base/Window.h"
#include "TG/TGManager.h"
#include "UI/TabManager.h"
#include "UI/Panels/TGPanel.h"

RuntimeLayer::RuntimeLayer()
: tg::Layer("RuntimeLayer")
{
    TG(LayerLog,Info,"RuntimeLayer created")
}

RuntimeLayer::~RuntimeLayer()
{
}

void RuntimeLayer::OnAttach()
{
    Layer::OnAttach();
    tg::TabManager::Get().Init();

    auto telegramPanel = std::make_shared<tg::TGPanel>();
    tg::TabManager::Get().AddPanel(telegramPanel);

    tg::TabManager::Get().SetActivePanel("Telegram");
    

}

void RuntimeLayer::OnDetach()
{
    tg::TabManager::Get().Shutdown();
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate(tg::TimeStep ts)
{
    Layer::OnUpdate(ts);
}

void RuntimeLayer::OnImGuiRender()
{

   ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    static bool dockspaceOpen = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("MainDockSpace", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);
    
    // Render Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Add Telegram Account"))
            {
                // Get the telegram panel and add an account programmatically if needed
                auto telegramPanel = tg::TabManager::Get().GetPanel("Telegram");
                if (telegramPanel)
                {
                    // This would require exposing a method to trigger the add account popup
                    // For now, users can use the + button in the panel
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit"))
            {
                // Application exit logic here
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::BeginMenu("Panels"))
            {
                // Add new Telegram panel instance
                if (ImGui::MenuItem("New Telegram Panel"))
                {
                    auto newPanel = std::make_shared<tg::TGPanel>();
                    tg::TabManager::Get().AddPanel(newPanel);
                }
                
                ImGui::Separator();
                
                // List all panels for quick access
                ImGui::Text("Active Panels:");
                ImGui::Separator();
                
                // You could iterate through panels here if TabManager exposed a method
                ImGui::Text("• Telegram");
                
                ImGui::EndMenu();
            }
            
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", nullptr, &mShowDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                mShowAboutWindow = true;
            }
            
            if (ImGui::MenuItem("Usage Guide"))
            {
                mShowUsageGuide = true;
            }
            
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    // Setup DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }
    
    ImGui::End();
    
    // Render the TabManager (this renders all panels and tabs)
    tg::TabManager::Get().Render();
    
    // Optional: Show ImGui demo window
    if (mShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&mShowDemoWindow);
    }
    
    // Show About Window
    if (mShowAboutWindow)
    {
        ImGui::Begin("About", &mShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Telegram Panel System");
        ImGui::Separator();
        ImGui::Text("A modular panel system with browser-like tabs");
        ImGui::Text("Built with ImGui");
        ImGui::Spacing();
        ImGui::Text("Features:");
        ImGui::BulletText("Browser-like tabs");
        ImGui::BulletText("Detachable panels");
        ImGui::BulletText("Telegram messaging interface");
        ImGui::BulletText("Multiple account support");
        ImGui::End();
    }
    
    // Show Usage Guide
    if (mShowUsageGuide)
    {
        ImGui::Begin("Usage Guide", &mShowUsageGuide);
        
        ImGui::TextWrapped("Welcome to the Telegram Panel System!");
        ImGui::Separator();
        
        ImGui::Text("Getting Started:");
        ImGui::BulletText("Click the '+' button in Telegram panel to add an account");
        ImGui::BulletText("Enter any phone number (it's mock data for now)");
        ImGui::BulletText("Double-click on any chat to open a chat window");
        
        ImGui::Spacing();
        ImGui::Text("Tab Features:");
        ImGui::BulletText("Drag tabs to reorder them");
        ImGui::BulletText("Right-click on tab to detach it");
        ImGui::BulletText("Click 'X' to close a tab");
        ImGui::BulletText("Detached windows can be reattached");
        
        ImGui::Spacing();
        ImGui::Text("Chat Features:");
        ImGui::BulletText("Search chats using the search bar");
        ImGui::BulletText("Send messages with Enter key or Send button");
        ImGui::BulletText("Pinned chats appear at the top");
        ImGui::BulletText("Online indicators show active users");
        
        ImGui::End();
    }
}
