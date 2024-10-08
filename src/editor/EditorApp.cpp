#include <application/Application.hpp>
#include <entry_point.hpp>

Zeus::Application* Zeus::CreateApplication()
{
    return new Zeus::Application(1440, 1080, "Zeus Editor");
}
