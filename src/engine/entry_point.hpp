#pragma once

#include "application/Application.hpp"

namespace Zeus
{
[[nodiscard]] extern Application* CreateApplication(CommandLineArgs args);
}

int main(int argc, char** argv);
