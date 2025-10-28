#include "RuntimeLayer.h"
#include <imgui.h>

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
}

void RuntimeLayer::OnDetach()
{
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate(tg::TimeStep ts)
{
    Layer::OnUpdate(ts);
}

void RuntimeLayer::OnImGuiRender()
{
    ImGui::ShowDemoWindow();
    
    // Or create your own simple window
    ImGui::Begin("Runtime Layer");
    ImGui::Text("Hello from Runtime Layer!");
    ImGui::Text("Frame time: %.3f ms", ImGui::GetIO().DeltaTime * 1000.0f);
    ImGui::End();
}
