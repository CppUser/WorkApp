#include "Base/App.h"

#include "Base/Log.h"
#include "Base/Window.h"

namespace tg
{
    App::App()
    {
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
        
    }

    App::~App()
    {
    }


    void App::Run()
    {
        while (mWindow->isOpen())
        {
            mWindow->ProcessEvents();

            mWindow->SwapBuffers();
        }
    }
}
