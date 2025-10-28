#pragma once
#include <memory>

#include "Layer.h"


namespace tg
{
    class App
    {
    public:
        App();
        ~App();

        virtual void OnInit(){}
        virtual void OnShutdown();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* layer);
        void RenderImGui();

        void Run();

        inline class Window& GetWindow() const{return *mWindow;}
        static inline App& Get() { return *sInstance; }
    private:
        std::unique_ptr<class Window> mWindow;

        LayerStack mLayerStack;
        class ImGuiLayer* mImGuiLayer;
        Timer mFrameTimer;
        TimeStep mTimeStep;
        float mLastFrameTime = 0.0f;

        static App* sInstance;
    };

    
    
}
extern tg::App* CreateApp();