#include "Base/Time.h"


namespace tg
{
    Timer::Timer()
    {
        Reset();
    }

    float Timer::Reset()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsedTime = std::chrono::duration<float>(currentTime - mStartTime).count();
        mStartTime = currentTime;
        return elapsedTime;
    }

    float Timer::GetElapsedTime() const
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float>(currentTime - mStartTime).count();
    }

    float Timer::GetElapsedSeconds() const
    {
        return GetElapsedTime();
    }

    float Timer::GetElapsedMilliseconds() const
    {
        return GetElapsedTime() * 1000.0f;
    }
}