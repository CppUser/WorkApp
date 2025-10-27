#include "Base/Log.h"


#include <filesystem>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"


namespace tg {

    std::shared_ptr<spdlog::logger> Log::sCoreLogger;
    std::shared_ptr<spdlog::logger> Log::sLayerLogger;


    void Log::Init() {
        //TODO: Проверь если папка уже создана , незачем вызывать эту команду снова
        //TODO: FIX На данный момент логи создаются в sandbox папке для Core и Layer
        std::filesystem::create_directories("logs");

        // ----- Core Logger (Core) -----
        auto coreConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // Pattern: time, logger name, source file and line, then the message.
        coreConsoleSink->set_pattern("%^[%T] [Core] [%s:%#] %v%$");

        auto coreFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Core.log", true);
        coreFileSink->set_pattern("[%T] [Core] [%s:%#] %v");

        std::vector<spdlog::sink_ptr> coreSinks { coreConsoleSink, coreFileSink };
        sCoreLogger = std::make_shared<spdlog::logger>("Engine", coreSinks.begin(), coreSinks.end());
        sCoreLogger->set_level(spdlog::level::trace);
        sCoreLogger->flush_on(spdlog::level::trace);

        // ----- Layer Logger -----
        auto layerConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        layerConsoleSink->set_pattern("%^[%T] [Layer] [%s:%#] %v%$");

        auto layerFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Layer.log", true);
        layerFileSink->set_pattern("[%T] [Layer] [%s:%#] %v");

        std::vector<spdlog::sink_ptr> layerSinks { layerConsoleSink, layerFileSink };
        sLayerLogger = std::make_shared<spdlog::logger>("Layer", layerSinks.begin(), layerSinks.end());
        sLayerLogger->set_level(spdlog::level::trace);
        sLayerLogger->flush_on(spdlog::level::trace);
        
    }

    void Log::Shutdown() {
        sCoreLogger.reset();
        sLayerLogger.reset();
        spdlog::drop_all();
    }
}
