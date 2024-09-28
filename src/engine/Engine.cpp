#include "Engine.hpp"

#include <chrono>
#include <cstdint>
#include <thread>

namespace Zeus
{
Engine::Engine(std::uint32_t width, std::uint32_t height, const char* title)
    : vkContext(width, height, title)
{
}

void Engine::Init()
{
    vkContext.Init();
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(vkContext.window.handle))
    {
        // auto start = std::chrono::system_clock::now();
        glfwPollEvents();

        // // Handle events on queue
        // while (SDL_PollEvent(&e) != 0)
        // {
        //     // close the window when user alt-f4s or clicks the X button
        //     if (e.type == SDL_QUIT)
        //         bQuit = true;
        //
        //     if (e.type == SDL_WINDOWEVENT)
        //     {
        //
        //         if (e.window.event == SDL_WINDOWEVENT_RESIZED)
        //         {
        //             resize_requested = true;
        //         }
        //         if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
        //         {
        //             freeze_rendering = true;
        //         }
        //         if (e.window.event == SDL_WINDOWEVENT_RESTORED)
        //         {
        //             freeze_rendering = false;
        //         }
        //     }
        //
        //     mainCamera.processSDLEvent(e);
        //     ImGui_ImplSDL2_ProcessEvent(&e);
        // }

        if (vkContext.window.invalidExtent) // freeze_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (vkContext.window.resized)
        {
            vkContext.RecreateSwapchain();
        }

        // // imgui new frame
        // ImGui_ImplVulkan_NewFrame();
        // ImGui_ImplSDL2_NewFrame();
        //
        // ImGui::NewFrame();
        //
        // ImGui::Begin("Stats");
        //
        // ImGui::Text("frametime %f ms", stats.frametime);
        // ImGui::Text("drawtime %f ms", stats.mesh_draw_time);
        // ImGui::Text("triangles %i", stats.triangle_count);
        // ImGui::Text("draws %i", stats.drawcall_count);
        // ImGui::End();
        //
        // if (ImGui::Begin("background"))
        // {
        //
        //     ComputeEffect& selected =
        //         backgroundEffects[currentBackgroundEffect];
        //
        //     ImGui::Text("Selected effect: ", selected.name);
        //
        //     ImGui::SliderInt(
        //         "Effect Index",
        //         &currentBackgroundEffect,
        //         0,
        //         backgroundEffects.size() - 1);
        //
        //     ImGui::InputFloat4("data1", (float*)&selected.data.data1);
        //     ImGui::InputFloat4("data2", (float*)&selected.data.data2);
        //     ImGui::InputFloat4("data3", (float*)&selected.data.data3);
        //     ImGui::InputFloat4("data4", (float*)&selected.data.data4);
        //
        //     ImGui::End();
        // }
        //
        // ImGui::Render();
        //
        // // imgui commands
        // // ImGui::ShowDemoWindow();
        //
        // update_scene();
        //
        // draw();
        //
        // auto end = std::chrono::system_clock::now();
        // auto elapsed =
        //     std::chrono::duration_cast<std::chrono::microseconds>(end -
        //     start);
        //
        // stats.frametime = elapsed.count() / 1000.f;
    }
}

void Engine::Shutdown()
{
    vkContext.Destroy();
}
}
