#include "Application.hpp"

#include "core/Timer.hpp"
#include "input/InputManager.hpp"
#include "logging/logger.hpp"
#include "rendering/Renderer.hpp"
#include "vulkan/VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cstdint>
#include <thread>

namespace Zeus
{
Application::Application(
    std::uint32_t width,
    std::uint32_t height,
    const char* title)
    : m_window(width, height, title),
      m_vkContext(),
      m_renderer(m_window, m_vkContext)
{
}

void Application::Init()
{
    m_window.Init();
    m_vkContext.Init(m_window);
    m_renderer.Init();

    InputManager::Init(m_window.handle);

    // uiManager.Init(m_window, m_vkContext);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_window.handle))
    {
        Timer timer;
        timer.Start();

        glfwPollEvents();

        if (m_window.invalidExtent)
        {
            glfwWaitEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (m_renderer.ResizeRequired())
        {
            m_renderer.ResizeDrawObjects(m_window.extent);
        }

        // uiManager.ConfigureFrame();

        // imgui commands
        // updateScene();

        m_renderer.Draw();

        timer.End();
        statistics.frametime = timer.GetMilliseconds();
        statistics.fps = 1000.f / statistics.frametime;
    }
}

void Application::Shutdown()
{
    LOG_DEBUG("Shutting down application");

    vkDeviceWaitIdle(m_vkContext.device.logicalDevice);

    // uiManager.Destroy(m_vkContext.device.logicalDevice);

    m_renderer.Destroy();
    m_vkContext.Destroy();
    m_window.Destroy();
}
}
