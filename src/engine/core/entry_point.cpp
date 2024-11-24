#include "entry_point.hpp"

#include "application/Application.hpp"

#include <cstdlib>

int main(int argc, char** argv)
{
    auto app{ Zeus::CreateApplication(Zeus::CommandLineArgs(argv, argc)) };

    app->Initialize();

    app->Run();

    app->Shutdown();

    delete app;

    return EXIT_SUCCESS;
}
