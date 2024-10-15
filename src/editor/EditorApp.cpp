#include <application/Application.hpp>
#include <entry_point.hpp>

Zeus::Application* Zeus::CreateApplication(CommandLineArgs args)
{
    ApplicationSpecification specification{
        .name = "Zeus Editor",
        .commandLineArgs = args,
        .windowWidth = 1440,
        .windowHeight = 1080,
    };

    return new Zeus::Application(specification);
}
