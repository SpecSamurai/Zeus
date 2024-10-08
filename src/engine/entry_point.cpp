#include "entry_point.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    auto app{ Zeus::CreateApplication() };

    app->Init();

    app->Run();

    app->Shutdown();

    return EXIT_SUCCESS;
}
