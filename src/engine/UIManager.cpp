#include "UIManager.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_dynamic_rendering.hpp"
#include "vulkan/vulkan_memory.hpp"
#include <vulkan/vulkan_core.h>

namespace Zeus
{
void UIManager::Init(const VulkanContext& vkContext)
{
    VkDescriptorPoolSize poolSizes[]{
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 1000;
    createInfo.poolSizeCount = std::size(poolSizes);
    createInfo.pPoolSizes = poolSizes;

    vkCreateDescriptorPool(
        vkContext.device.logicalDevice,
        &createInfo,
        allocationCallbacks.get(),
        &uiDescriptorPool);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_DESCRIPTOR_POOL,
        reinterpret_cast<std::uint64_t>(uiDescriptorPool),
        "DescriptorPool UI");
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForVulkan(vkContext.window.handle, true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = vkContext.instance.handle;
    initInfo.PhysicalDevice = vkContext.device.physicalDevice;
    initInfo.Device = vkContext.device.logicalDevice;
    initInfo.QueueFamily = vkContext.device.graphicsFamily;
    initInfo.Queue = vkContext.device.graphicsQueue;
    // init_info.PipelineCache = YOUR_PIPELINE_CACHE;
    initInfo.DescriptorPool = uiDescriptorPool;
    // init_info.Subpass = 0;
    initInfo.MinImageCount = vkContext.swapchain.imageCount;
    initInfo.ImageCount = vkContext.swapchain.imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.UseDynamicRendering = true;

    initInfo.PipelineRenderingCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &vkContext.swapchain.imageFormat;

    initInfo.Allocator = allocationCallbacks.get();

    ImGui_ImplVulkan_Init(&initInfo);
    ImGui_ImplVulkan_CreateFontsTexture();

    // ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void UIManager::Draw(
    VkCommandBuffer commandBuffer,
    const VkRenderingAttachmentInfo& colorAttachment,
    const VkExtent2D& renderExtent)
{
    cmdBeginRendering(commandBuffer, renderExtent, 1, &colorAttachment);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    vkCmdEndRendering(commandBuffer);
}

void UIManager::Destroy(VkDevice device)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(
        device,
        uiDescriptorPool,
        allocationCallbacks.get());
}

void UIManager::ConfigureFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool opened{ true };
    const ImGuiViewport* viewport{ ImGui::GetMainViewport() };

    ShowStatistics(viewport, opened);

    ImGuiWindowFlags windowFlags{
        ImGuiWindowFlags_NoSavedSettings |
            //| ImGuiWindowFlags_NoTitleBar,
            ImGuiWindowFlags_MenuBar,
    };

    // ImGui::SetNextWindowPos(viewport->GetWorkCenter(),
    // ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f));
    if (ImGui::Begin("Main Window", &opened, windowFlags))
    {
        ImGui::PopStyleVar();
        ShowMenuBar();

        if (ImGui::BeginChild(
                "Row 1",
                ImVec2(0, ImGui::GetContentRegionAvail().y),
                ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY,
                ImGuiWindowFlags_NoSavedSettings))
        {
            // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f,
            // 0.f)); if (ImGui::BeginChild(
            //         "Viewport",
            //         ImVec2(
            //             ImGui::GetContentRegionAvail().x * 0.7f,
            //             ImGui::GetContentRegionAvail().y),
            //         ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX,
            //         ImGuiWindowFlags_NoSavedSettings))
            // {
            //
            //     // ImGuiIO& io{ ImGui::GetIO() };
            //     // ImTextureID my_tex_id = io.Fonts->TexID;
            //     // float my_tex_w = (float)io.Fonts->TexWidth;
            //     // float my_tex_h = (float)io.Fonts->TexHeight;
            //     // ImVec2 uv_min = ImVec2(0.0f, 0.0f);
            //     // ImVec2 uv_max = ImVec2(1.0f, 1.0f);
            //     // ImVec4 tint_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            //     // ImVec4 border_col =
            //     // ImGui::GetStyleColorVec4(ImGuiCol_Border); ImGui::Image(
            //     //     my_tex_id,
            //     //     ImGui::GetWindowSize(),
            //     //     uv_min,
            //     //     uv_max,
            //     //     tint_col,
            //     //     border_col);
            //
            //     ImGui::EndChild();
            // }
            // ImGui::PopStyleVar();
            //
            // ImGui::SameLine();
            //
            // if (ImGui::BeginChild(
            //         "Tree",
            //         ImGui::GetContentRegionAvail(),
            //         ImGuiChildFlags_None,
            //         ImGuiWindowFlags_NoSavedSettings))
            // {
            //     // if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            //     // {
            //     //     if (ImGui::BeginTabItem("Description"))
            //     //     {
            //     //         ImGui::TextWrapped(
            //     //             "Lorem ipsum dolor sit amet, consectetur");
            //     //         ImGui::EndTabItem();
            //     //     }
            //     //     if (ImGui::BeginTabItem("Details"))
            //     //     {
            //     //         ImGui::Text("ID: 0123456789");
            //     //         ImGui::EndTabItem();
            //     //     }
            //     //     ImGui::EndTabBar();
            //     // }
            //
            //     ImGui::EndChild();
            // }
        }
        ImGui::EndChild();

        if (ImGui::BeginChild(
                "Row 2",
                ImVec2(0, 150),
                ImGuiChildFlags_None,
                ImGuiWindowFlags_NoSavedSettings))
        {
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    ImGui::TextWrapped("Lorem ipsum dolor sit amet");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        // if (ImGui::Begin("background")
        // {
        // ComputeEffect& selected = backgroundEffects[0];

        // ImGui::Text("Selected effect: ", selected.name);

        // ImGui::SliderInt(
        //     "Effect Index",
        //     &,
        //     0,
        //     backgroundEffects.size() - 1);

        // ImGui::InputFloat4("data1", (float*)&selected.data.data1);
        // ImGui::InputFloat4("data2", (float*)&selected.data.data2);
        // ImGui::InputFloat4("data3", (float*)&selected.data.data3);
        // ImGui::InputFloat4("data4", (float*)&selected.data.data4);

        //     ImGui::End();
        // }
        ImGui::End();
    }
    ImGui::PopStyleVar();

    if (showDemoWindow)
        ImGui::ShowDemoWindow();

    // ImGui::Render();
}

void UIManager::ShowStatistics(const ImGuiViewport* viewport, bool opened)
{
    ImGuiWindowFlags windowFlags{
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav,
    };

    ImGui::SetNextWindowPos(ImVec2(1.f, 1.f), ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Engine Statistics", &opened, windowFlags))
    {
        ImGui::Text("Frametime %.4f ms", 100.f);
        ImGui::Text("FPS %.4f", 100.f);
        ImGui::Text("Draw time %.4f ms", 100.f);
        ImGui::Text("Update time %.4f ms", 100.f);
        ImGui::Text("Triangles %i", 100);
        ImGui::Text("Draws %i", 100);
    }
    ImGui::End();
}

void UIManager::ShowMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("(demo menu)", NULL, false, false);
            if (ImGui::MenuItem("New"))
            {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
            }
            if (ImGui::BeginMenu("Open Recent"))
            {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
            }
            if (ImGui::MenuItem("Save As.."))
            {
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Demo Window", nullptr, showDemoWindow))
            {
                showDemoWindow ^= 1;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("About", nullptr, false))
            {
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}
}
