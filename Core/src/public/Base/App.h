#pragma once
#include <memory>


namespace tg
{
    class App
    {
    public:
        App();
        ~App();

        void Run();
    private:
        std::unique_ptr<class Window> mWindow;
    };

    
    
}
extern tg::App* CreateApp();