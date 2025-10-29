#pragma once
#include <Core.h>

class RuntimeLayer : public tg::Layer
{
public:
    RuntimeLayer();
    ~RuntimeLayer() override;
    
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(tg::TimeStep ts) override;
    void OnImGuiRender() override;
private:
    bool mShowDemoWindow = false;
    bool mShowAboutWindow = false;
    bool mShowUsageGuide = false;
};


