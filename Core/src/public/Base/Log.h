#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace tg 
{
    class Log
    {
    public:
        static void Init();
        static void Shutdown();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetLayerLogger() { return sLayerLogger; }

        static inline const char* GetFunctionName(const char* func) { return func; }
    private:
        static std::shared_ptr<spdlog::logger> sCoreLogger;
        static std::shared_ptr<spdlog::logger> sLayerLogger;
    };
}

//engine logger macros
#define TG_CoreLog_Trace(...) (tg::Log::GetCoreLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::info, __VA_ARGS__);
#define TG_CoreLog_Info(...) (tg::Log::GetCoreLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::info, __VA_ARGS__);
#define TG_CoreLog_Warn(...) (tg::Log::GetCoreLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::warn, __VA_ARGS__);
#define TG_CoreLog_Error(...) (tg::Log::GetCoreLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::err, __VA_ARGS__);
#define TG_CoreLog_Fatal(...) (tg::Log::GetCoreLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::critical, __VA_ARGS__);

//game logger macros
#define TG_LayerLog_Trace(...) (tg::Log::GetLayerLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::trace, __VA_ARGS__);
#define TG_LayerLog_Info(...) (tg::Log::GetLayerLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::info, __VA_ARGS__);
#define TG_LayerLog_Warn(...) (tg::Log::GetLayerLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::warn, __VA_ARGS__);
#define TG_LayerLog_Error(...) (tg::Log::GetLayerLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::err, __VA_ARGS__);
#define TG_LayerLog_Fatal(...) (tg::Log::GetLayerLogger())->log(spdlog::source_loc{__FILE__, __LINE__, tg::Log::GetFunctionName(__func__)}, spdlog::level::critical, __VA_ARGS__);

#define TG(module, lvl, ...) TG_##module##_##lvl(__VA_ARGS__);