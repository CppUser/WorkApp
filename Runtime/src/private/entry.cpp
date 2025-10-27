#include <Base/Entry.h>

#include "RuntimeLayer.h"

class Runtime : public tg::App
{
public:
    Runtime() : App() {}

    void OnInit() override
    {
        PushLayer(new RuntimeLayer());
    }
};

tg::App* CreateApp()
{
    
    return new Runtime();
}