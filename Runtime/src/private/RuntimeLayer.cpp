#include "RuntimeLayer.h"

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
    Layer::OnImGuiRender();
}
