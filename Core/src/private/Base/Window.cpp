#include "Base/Window.h"
#include "Base/Log.h"
#include "glad/glad.h"
#include <glfw/glfw3.h>
namespace tg
{
    namespace
    {
        uint32_t sGLFWWindowCount = 0;

        void GLFWErrorCallback(int error, const char* description)
        {
            TG(CoreLog,Error,"GLFW Error ({0}): {1}", error, description)
        }
        
    }
    
    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init()
    {
        if (sGLFWWindowCount == 0)
        {
            if (!glfwInit()) {
                const char* description;
                glfwGetError(&description);
                TG(CoreLog, Error, "GLFW Init Error: {}", description);
                throw std::runtime_error("Could not initialize GLFW");
            }
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        if (mConfig.dimensions.startPosX > 0 || mConfig.dimensions.startPosY > 0)
        {
            glfwWindowHint(GLFW_POSITION_X, static_cast<int>(mConfig.dimensions.startPosX));
            glfwWindowHint(GLFW_POSITION_Y, static_cast<int>(mConfig.dimensions.startPosY));
        }

        //TODO: This context forces opengl
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        glfwWindowHint(GLFW_DECORATED, mConfig.decorated ? GLFW_TRUE : GLFW_FALSE);

        if (mConfig.transparent) {
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        }

        TG(CoreLog, Info, "Creating window: {}x{} at ({}, {})",
           mConfig.dimensions.width, mConfig.dimensions.height,
           mConfig.dimensions.startPosX, mConfig.dimensions.startPosY)

        switch (mConfig.mode)
        {
        case Mode::Fullscreen: {
                auto* monitor = glfwGetPrimaryMonitor();
                const auto* mode = glfwGetVideoMode(monitor);
                mWindow = glfwCreateWindow(mode->width, mode->height, mConfig.title.c_str(), monitor, nullptr);
        } break;
        case Mode::FullscreenBorderless: {
                auto* monitor = glfwGetPrimaryMonitor();
                const auto* mode = glfwGetVideoMode(monitor);
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                mWindow = glfwCreateWindow(mode->width, mode->height, mConfig.title.c_str(), monitor, nullptr);
        } break;
        default: {
                int width = static_cast<int>(mConfig.dimensions.width);
                int height = static_cast<int>(mConfig.dimensions.height);
                mWindow = glfwCreateWindow(width, height, mConfig.title.c_str(), nullptr, nullptr);
        }
        }

        if (!mWindow) {
            TG(CoreLog, Error, "Failed to create GLFW window")
            const char* description;
            glfwGetError(&description);
            TG(CoreLog, Error, "GLFW Error: {}", description)
            throw std::runtime_error("Could not create window");
        }
        ++sGLFWWindowCount;

        glfwMakeContextCurrent(mWindow); //TODO: Move to context class
        glfwSwapInterval(mConfig.vsync == VSync::On ? 1 : 0); //TODO: Check in func IsVSync

        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); //TODO: Move to context class
        if (!status)
        {
            TG(CoreLog, Error, "Failed to initialize GLAD");
            throw std::runtime_error("Could not initialize GLAD");
        }

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(mWindow, &fbWidth, &fbHeight); //TODO: Move to context class or framebuffer class
        glViewport(0, 0, fbWidth, fbHeight);

        glfwSetWindowUserPointer(mWindow, this);
    }

    void Window::ProcessEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(mWindow); //TODO: for other api's this done differently
    }

    bool Window::isOpen() const
    {
        return !glfwWindowShouldClose(mWindow);
    }

    
    void Window::Shutdown()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }

        if (sGLFWWindowCount > 0)
        {
            --sGLFWWindowCount;
            if (sGLFWWindowCount == 0)
            {
                glfwTerminate();
            }
        }
    }

    Window* Window::Create(const WindowConfig& config)
    {
        return new Window(config);
    }
}
