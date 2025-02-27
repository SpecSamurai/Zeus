#pragma once

#include "CommandLineArgs.hpp"
#include "window/Window.hpp"

#include <cstdint>

namespace Zeus
{
struct ApplicationSpecification
{
    const char* name;
    CommandLineArgs commandLineArgs;
    std::uint32_t windowWidth;
    std::uint32_t windowHeight;
};

class Application
{
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application() = default;

    virtual void Initialize() = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;

    static const Application& Instance();
    const Window& GetWindow() const;

private:
    static Application* s_instance;

protected:
    Window m_window;
};

[[nodiscard]] extern Application* CreateApplication(CommandLineArgs args);
}
