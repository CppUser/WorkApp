#pragma once
#include "App.h"
#include "Log.h"


int main(int argc, char** argv)
{
    tg::Log::Init();
    auto app = CreateApp();
    app->Run();

    tg::Log::Shutdown();
    return 0;
} 