#include "Application.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Zeus::Application app;

    app.Init(1440, 1080, "Zeus Engine");

    app.Run();

    app.Shutdown();

    return EXIT_SUCCESS;
}
