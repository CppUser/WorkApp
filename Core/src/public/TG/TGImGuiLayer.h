#pragma once
#include "ImGui/ImGuiLayer.h"

#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API
#endif
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

namespace tg
{
    class TGImGuiLayer : public ImGuiLayer
    {
    public:
        TGImGuiLayer();
        TGImGuiLayer(const std::string& name);
        virtual ~TGImGuiLayer();

        virtual void Begin() override;
        virtual void End() override;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
    private:
        float mTime = 0.0f;
    };
}
