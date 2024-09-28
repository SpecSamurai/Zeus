#include "Application.hpp"
#include "Engine.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Zeus::Engine app(1440, 1080, "Zeus Editor");

    app.Init();

    app.Run();

    app.Shutdown();

    return EXIT_SUCCESS;
}
