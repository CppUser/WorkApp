#include <Core.h>
#include <Base/Entry.h>

class Runtime : public tg::App
{
public:
    Runtime() : App() {}
};

tg::App* CreateApp()
{
    
    return new Runtime();
}