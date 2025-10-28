#pragma once
#include <cstdint>
#include <string>

struct GLFWwindow;

namespace tg
{
    struct Dimensions
    {
        uint32_t startPosX = 0;
        uint32_t startPosY = 0;
        uint32_t width = 1280;
        uint32_t height = 720;
    };

    enum class Mode : uint8_t { Headless,Fullscreen,FullscreenBorderless,Default};
    enum class VSync {Off,On};

    struct WindowConfig
    {
        std::string title{ "TG" };
        Mode mode{ Mode::Default };
        Dimensions dimensions{};
        bool decorated = false;
        bool transparent = false;
        VSync vsync = VSync::On;
    };

    class Window {
    public:
        explicit Window(WindowConfig   config): mConfig(std::move(config)) {}
        virtual ~Window();

        void Init();
        void ProcessEvents();
        void SwapBuffers();

        [[nodiscard]] bool isOpen() const;
        [[nodiscard]] uint32_t GetWidth() const { return mConfig.dimensions.width; }
        [[nodiscard]] uint32_t GetHeight() const { return mConfig.dimensions.height; }
        [[nodiscard]] inline void* GetNativeWindow() const { return mWindow; }

        virtual std::pair<uint32_t, uint32_t> GetSize() const { return { mConfig.dimensions.width, mConfig.dimensions.height }; }

        static Window* Create(const WindowConfig& config = WindowConfig());

    private:
        void Shutdown();
    private:
        GLFWwindow* mWindow = nullptr;
        WindowConfig mConfig;

    };

    
}
