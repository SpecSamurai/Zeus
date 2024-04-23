#include "Application.hpp"

constexpr int WIDTH{800};
constexpr int HEIGHT{600};
constexpr const char* TITLE{"Zeus"};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Zeus::Application app;
    app.Run(WIDTH, HEIGHT, TITLE);

    return 0;
}
