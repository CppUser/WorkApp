#pragma once

#include <chrono>

namespace tg
{
    class Timer
    {
    public:
        Timer();

        float Reset();

        float GetElapsedTime() const;
        float GetElapsedSeconds() const;
        float GetElapsedMilliseconds() const;

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
    };

    class TimeStep
    {
    public:
        TimeStep(float time = 0.0f) : mTime(time) {}

        float GetSeconds() const { return mTime; }
        float GetMilliseconds() const { return mTime * 1000.0f; }
        operator float() const { return mTime; }

    private:
        float mTime;
    };
}
