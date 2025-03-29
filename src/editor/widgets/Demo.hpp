#pragma once

#include "assets/AssetsManager.hpp"
#include "core/Engine.hpp"
#include "widgets/Widget.hpp"

#include <imgui.h>

namespace Zeus
{
class Demo : public Widget
{
public:
    void Update() override
    {
        // >>>>>>>>>>
        /*auto asset = ModelLoader::GetObjLoader()->Load(*/
        /*    "D:/Code/Zeus/models/robot/model.obj");*/
        /**/
        /*auto asset2 = ModelLoader::GetObjLoader()->Load(*/
        /*    "D:/Code/Zeus/models/kalestra-the-sorceress/source/Pose_Body.obj");*/
        /**/
        /*asset->materials["base"] = std::make_shared<Material>();*/
        /**/
        /*Image baseColor =*/
        /*    Image::LoadFromFile("D:/Code/Zeus/models/robot/color.jpg");*/
        /*asset->materials["base"]->SetTexture(TextureType::BASE_COLOR,
         * &baseColor);*/
        /**/
        /*Image emission =*/
        /*    Image::LoadFromFile("D:/Code/Zeus/models/robot/emission.jpeg");*/
        /*asset->materials["base"]->SetTexture(TextureType::EMISSION,
         * &emission);*/
        /**/
        /*Image roughness =*/
        /*    Image::LoadFromFile("D:/Code/Zeus/models/robot/roughness.jpeg");*/
        /*asset->materials["base"]->SetTexture(TextureType::ROUGHNESS,
         * &roughness);*/
        /**/
        /*Image normal =
         * Image::LoadFromFile("D:/Code/Zeus/models/robot/normal.jpg");*/
        /*asset->materials["base"]->SetTexture(TextureType::NORMAL, &normal);*/
        /**/
        /*Engine::Renderer().m_materialSet.Update(*/
        /*    {},*/
        /*    {*/
        /*        DescriptorImage(*/
        /*            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,*/
        /*            VK_SHADER_STAGE_FRAGMENT_BIT,*/
        /*            0,*/
        /*            baseColor.GetView(),*/
        /*            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,*/
        /*            Engine::Renderer()*/
        /*                .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)*/
        /*                .GetHandle()),*/
        /*        DescriptorImage(*/
        /*            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,*/
        /*            VK_SHADER_STAGE_FRAGMENT_BIT,*/
        /*            1,*/
        /*            normal.GetView(),*/
        /*            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,*/
        /*            Engine::Renderer()*/
        /*                .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)*/
        /*                .GetHandle()),*/
        /*        DescriptorImage(*/
        /*            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,*/
        /*            VK_SHADER_STAGE_FRAGMENT_BIT,*/
        /*            2,*/
        /*            emission.GetView(),*/
        /*            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,*/
        /*            Engine::Renderer()*/
        /*                .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)*/
        /*                .GetHandle()),*/
        /*        DescriptorImage(*/
        /*            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,*/
        /*            VK_SHADER_STAGE_FRAGMENT_BIT,*/
        /*            3,*/
        /*            roughness.GetView(),*/
        /*            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,*/
        /*            Engine::Renderer()*/
        /*                .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)*/
        /*                .GetHandle()),*/
        /*    });*/

        /*auto asset2 = ModelLoader::GetObjLoader()->Load(*/
        /*"D:/Code/Zeus/models/FinalBaseMesh.obj");*/
        /*"D:/Code/Zeus/models/kalestra-the-sorceress/source/Pose_Body.obj");*/
        /*"D:/Code/Zeus/models/scifi-girl-v01/source/girl_complete_03.obj");*/

        /*for (auto& mesh : asset->meshes)*/
        /*{*/
        /*vec.emplace_back(new Renderable{*/
        /*    .m_mesh = mesh.second,*/
        /*    .m_material = asset->materials["base"].get(),*/
        /*    .localMatrix = Math::scale<float>(0.1f),*/
        /*});*/

        /*    Engine::World().Registry().Emplace<Renderable>(*/
        /*        0,*/
        /*        Renderable{*/
        /*            .m_mesh = mesh.second,*/
        /*            .m_material = asset->materials["base"].get(),*/
        /*            .localMatrix = Math::scale<float>(0.1f),*/
        /*        });*/
        /*}*/
        /**/
        /*Engine::World().Registry().Emplace<Renderable>(*/
        /*    1,*/
        /*    Renderable{*/
        /*        .m_mesh = (*asset->meshes.begin()).second,*/
        /*        .m_material = asset->materials["base"].get(),*/
        /*        .localMatrix = Math::scale<float>(0.1f) **/
        /*                       Math::translation(Math::Vector3f(10, 0, 0)),*/
        /*    });*/

        // >>>>>>>>>>

        ImGui::ShowDemoWindow();

        if (ImGui::Button("S", ImVec2(25, 25)))
        {
            auto asset = ModelLoader::GetObjLoader()->Load(
                "D:/Code/Zeus/models/robot/model.obj");

            asset->materials["base"] = std::make_shared<Material>();

            Image baseColor =
                Image::LoadFromFile("D:/Code/Zeus/models/robot/color.jpg");
            asset->materials["base"]->SetTexture(
                TextureType::BASE_COLOR,
                &baseColor);

            Image emission =
                Image::LoadFromFile("D:/Code/Zeus/models/robot/emission.jpeg");
            asset->materials["base"]->SetTexture(
                TextureType::EMISSION,
                &emission);

            Image roughness =
                Image::LoadFromFile("D:/Code/Zeus/models/robot/roughness.jpeg");
            asset->materials["base"]->SetTexture(
                TextureType::ROUGHNESS,
                &roughness);

            Image normal =
                Image::LoadFromFile("D:/Code/Zeus/models/robot/normal.jpg");
            asset->materials["base"]->SetTexture(TextureType::NORMAL, &normal);

            Engine::Renderer().m_materialSet.Update(
                {},
                {
                    DescriptorImage(
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        0,
                        baseColor.GetView(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        Engine::Renderer()
                            .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)
                            .GetHandle()),
                    DescriptorImage(
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        1,
                        normal.GetView(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        Engine::Renderer()
                            .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)
                            .GetHandle()),
                    DescriptorImage(
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        2,
                        emission.GetView(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        Engine::Renderer()
                            .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)
                            .GetHandle()),
                    DescriptorImage(
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        3,
                        roughness.GetView(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        Engine::Renderer()
                            .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)
                            .GetHandle()),
                });

            for (auto& mesh : asset->meshes)
            {
                /*vec.emplace_back(new Renderable{*/
                /*    .m_mesh = mesh.second,*/
                /*    .m_material = asset->materials["base"].get(),*/
                /*    .localMatrix = Math::scale<float>(0.1f),*/
                /*});*/

                Engine::World().Registry().Emplace<Renderable>(
                    0,
                    Renderable{
                        .m_mesh = mesh.second,
                        .m_material = asset->materials["base"].get(),
                        .localMatrix = Math::scale<float>(0.1f),
                    });
            }
        }
    }
};
}
