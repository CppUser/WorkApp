#pragma once


namespace tg
{
    class App
    {
    public:
        App();
        ~App();

        void PreInit();

        void Run();
        
    };

    
    
}
extern tg::App* CreateApp();