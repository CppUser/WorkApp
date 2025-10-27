#include "Base/App.h"

#include "Base/Log.h"

namespace tg
{
    App::App()
    {
        TG(CoreLog,Info,"App created")
    }

    App::~App()
    {
       
    }

    void App::PreInit()
    {
        
    }

    void App::Run()
    {
        while (true)
        {
            // TODO: Implement
        }
    }
}
