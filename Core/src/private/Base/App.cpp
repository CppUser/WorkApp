#include "Base/App.h"

#include "Base/Log.h"
#include "Base/Window.h"
#include "ImGui/ImGuiLayer.h"

namespace tg
{
    App* App::sInstance = nullptr;
    
    App::App()
    {
        if (sInstance)
        {
            TG(CoreLog, Error, "App already exists!")
        }
        sInstance = this;
        
        TG(CoreLog,Info,"App created")

        WindowConfig winCfg;
        winCfg.title = "TG";
        winCfg.mode = Mode::Default;
        winCfg.dimensions = {
            .startPosX = 0, .startPosY = 0,
            .width = 1280, .height = 720 };
        winCfg.decorated = true;
        winCfg.transparent = false;
        winCfg.vsync = VSync::On;

        mWindow = std::unique_ptr<Window>(Window::Create(winCfg));
        mWindow->Init();

        mImGuiLayer = ImGuiLayer::Create();
        PushOverlay(mImGuiLayer);
        
    }

    App::~App()
    {
        for (auto it = mLayerStack.begin(); it != mLayerStack.end();) {
            Layer* layer = *it;
            if (layer) {
                try {
                    layer->OnDetach();
                } catch (const std::exception& e) {
                    TG(CoreLog, Error, "Exception during layer cleanup: {}", e.what())
                } catch (...) {
                    TG(CoreLog, Error, "Unknown exception during layer cleanup")
                }
            } else {
                TG(CoreLog, Warn, "App destructor - Null layer in layer stack")
            }
            ++it;
        }
        mLayerStack.Clear();
    }

    void App::OnShutdown()
    {
    }

    void App::PushLayer(Layer* layer)
    {
        mLayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void App::PushOverlay(Layer* layer)
    {
        mLayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void App::PopLayer(Layer* layer)
    {
        mLayerStack.PopLayer(layer);
        layer->OnDetach();
    }

    void App::PopOverlay(Layer* layer)
    {
        mLayerStack.PopOverlay(layer);
        layer->OnDetach();
    }

    void App::RenderImGui()
    {
        mImGuiLayer->Begin();

        for (auto& layer : mLayerStack)
        {
            if (layer && layer->IsActive())
            {
                layer->OnImGuiRender();
            }
        }
    }


    void App::Run()
    {
        OnInit();
        mFrameTimer.Reset();
        while (mWindow->isOpen())
        {
            float currentFrameTime = mFrameTimer.Reset();
            mTimeStep = TimeStep(currentFrameTime);
            
            mWindow->ProcessEvents();

            mLastFrameTime = mTimeStep;
            for (auto& layer : mLayerStack)
            {
                if (layer && layer->IsActive())
                {
                    layer->OnUpdate(mTimeStep);
                }
            }

            App* app = this;
            app->RenderImGui();
            app->mImGuiLayer->End();
            
            mWindow->SwapBuffers();
        }
        OnShutdown();
    }
}
