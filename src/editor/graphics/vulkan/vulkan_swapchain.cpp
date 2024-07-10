#include "vulkan_swapchain.hpp"

#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>

namespace Zeus
{
bool createVulkanSwapchain(
    const VulkanDevice& vulkanDevice,
    const VkSurfaceKHR& surface,
    GLFWwindow* window,
    VulkanSwapchain& vulkanSwapchain)
{
    SurfaceDetails surfaceDetails =
        getSurfaceDetails(vulkanDevice.physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat =
        selectSurfaceFormat(surfaceDetails.formats);
    VkPresentModeKHR presentMode =
        selectPresentMode(surfaceDetails.presentModes);
    VkExtent2D extent = selectExtent(surfaceDetails.capabilities, window);

    // One more image for triple buffering or simply for more frames in flight
    std::uint32_t imageCount = surfaceDetails.capabilities.minImageCount + 1;
    // 0 is a special value that means that there is no maximum
    if (surfaceDetails.capabilities.maxImageCount > 0 &&
        imageCount > surfaceDetails.capabilities.maxImageCount)
    {
        imageCount = surfaceDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    // we can't use format from one pair and colorspace from another pair
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    // Defines the number of layers in a swap chain images (that is, views);
    // typically this value will be one but if we want to create multiview
    // or stereo (stereoscopic 3D) images, we can set it to some higher
    // value.
    createInfo.imageArrayLayers = 1; // always 1 unless you are developing a
                                     // stereoscopic 3D application
    // The imageUsage bit field specifies what kind of operations we'll use
    // the images in the swap chain for. In this tutorial we're going to
    // render directly to them, which means that they're used as color
    // attachment. It is also possible that you'll render images to a
    // separate image first to perform operations like post-processing. In
    // that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT
    // instead and use a memory operation to transfer the rendered image to
    // a swap chain image.
    // Usage flags define how a given image may be used in Vulkan. If we
    // want an image to be sampled (used inside shaders) it must be created
    // with “sampled” usage. If the image should be used as a depth render
    // target, it must be created with “depth and stencil” usage. An image
    // without proper usage “enabled” cannot be used for a given purpose or
    // the results of such operations will be undefined.

    // For a swap chain we want to render (in most cases) into the image
    // (use it as a render target), so we must specify “color attachment”
    // usage with VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT enum. In Vulkan this
    // usage is always available for swap chains, so we can always set it
    // without any additional checking. But for any other usage we must
    // ensure it is supported – we can do this through a
    // “supportedUsageFlags” member of surface capabilities structure.
    // “transfer destination” usage which is required for image clear
    // operation.
    // // Color attachment flag must always be supported
    // We can define other usage flags but we always need to check if they are
    // supported
    // In this example we define additional “transfer destination” usage which
    // is required for image clear operation. if
    // (surface_capabilities.supportedUsageFlags &
    //     VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    // {
    //     return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
    //            VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // }
    // std::cout << "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by
    // "
    //              "the swap chain!"
    //           << std::endl
    //           << "Supported swap chain's image usages include:" << std::endl
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSFER_SRC\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSFER_DST_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSFER_DST\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_SAMPLED_BIT
    //                   ? "    VK_IMAGE_USAGE_SAMPLED\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_STORAGE_BIT
    //                   ? "    VK_IMAGE_USAGE_STORAGE\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_COLOR_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_INPUT_ATTACHMENT"
    //                   : "")
    //           << std::endl;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::uint32_t queueFamilyIndices[] = {
        vulkanDevice.queueFamilies.graphicsFamily.value(),
        vulkanDevice.queueFamilies.presentFamily.value()
    };

    // Next, we need to specify how to handle swap chain images that will be
    // used across multiple queue families. That will be the case in our
    // application if the graphics queue family is different from the
    // presentation queue. We'll be drawing on the images in the swap chain
    // from the graphics queue and then submitting them on the presentation
    // queue. There are two ways to handle images that are accessed from
    // multiple queues:
    if (vulkanDevice.queueFamilies.graphicsFamily !=
        vulkanDevice.queueFamilies.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    // We can specify that a certain transform should be applied to images
    // in the swap chain if it is supported (supportedTransforms in
    // capabilities), like a 90 degree clockwise rotation or horizontal
    // flip. To specify that you do not want any transformation, simply
    // specify the current transformation.
    // On some platforms we may want our image to be transformed. This is
    // usually the case on tablets when they are oriented in a way other
    // than their default orientation. During swap chain creation we must
    // specify what transformations should be applied to images prior to
    // presenting. We can, of course, use only the supported transforms,
    // which can be found in a “supportedTransforms” member of acquired
    // surface capabilities.

    // If the selected pre-transform is other than the current
    // transformation (also found in surface capabilities) the presentation
    // engine will apply the selected transformation. On some platforms this
    // may cause performance degradation (probably not noticeable but worth
    // mentioning). In the sample code, I don't want any transformations
    // but, of course, I must check whether it is supported. If not, I'm
    // just using the same transformation that is currently used.
    // Here we don't want any transformations to occur so if the identity
    // transform is supported use it
    // otherwise just use the same transform as current transform
    // // Sometimes images must be transformed before they are presented (i.e.
    // due to device's orienation
    // being other than default orientation)
    // If the specified transform is other than current transform, presentation
    // engine will transform image during presentation operation; this operation
    // may hit performance on some platforms Here we don't want any
    // transformations to occur so if the identity transform is supported use it
    // otherwise just use the same transform as current transform
    // if (surface_capabilities.supportedTransforms &
    //     VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    // {
    //     return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    // }
    // else
    // {
    //     return surface_capabilities.currentTransform;
    // }
    createInfo.preTransform = surfaceDetails.capabilities.currentTransform;

    // The compositeAlpha field specifies if the alpha channel should be
    // used for blending with other windows in the window system. You'll
    // almost always want to simply ignore the alpha channel, hence
    // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR. When this flag is set, the alpha
    // channel of the presented image is treated as opaque (fully opaque),
    // meaning that the image is considered to completely cover the contents
    // of the underlying surface.
    // This mode is useful when the application knows that the rendered
    // image completely fills the surface and does not require alpha
    // blending with the contents beneath it.
    // This parameter is used to indicate how the surface (image) should be
    // composited (blended?) with other surfaces on some windowing systems;
    // this value must also be one of the possible values (bits) returned in
    // surface capabilities, but it looks like opaque composition (no
    // blending, alpha ignored) will be always supported (as most of the
    // games will want to use this mode).
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;

    // If the clipped member is set to VK_TRUE then that means that we don't
    // care about the color of pixels that are obscured, for example because
    // another window is in front of them. Unless you really need to be able
    // to read these pixels back and get predictable results, you'll get the
    // best performance by enabling clipping.
    createInfo.clipped = VK_TRUE;

    // auto oldSwapchain = vulkanSwapchain.handle;
    // createInfo.oldSwapchain = oldSwapchain; // VK_NULL_HANDLE;

    VkResult result{
        vkCreateSwapchainKHR(
            vulkanDevice.logicalDevice,
            &createInfo,
            nullptr,
            &vulkanSwapchain.handle),
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create swap chain. {}", vkResultToString(result));
        return false;
    }

    // if (oldSwapchain != VK_NULL_HANDLE)
    // {
    //     vkDestroySwapchainKHR(
    //         vulkanDevice.logicalDevice,
    //         oldSwapchain,
    //         nullptr);
    // }

    vkGetSwapchainImagesKHR(
        vulkanDevice.logicalDevice,
        vulkanSwapchain.handle,
        &imageCount,
        nullptr);

    vulkanSwapchain.swapchainImages.resize(imageCount);

    vkGetSwapchainImagesKHR(
        vulkanDevice.logicalDevice,
        vulkanSwapchain.handle,
        &imageCount,
        vulkanSwapchain.swapchainImages.data());

    vulkanSwapchain.imageFormat = surfaceFormat.format;
    vulkanSwapchain.extent = extent;

    vulkanSwapchain.swapchainImageViews.resize(
        vulkanSwapchain.swapchainImages.size());

    for (std::uint32_t i{ 0 }; i < vulkanSwapchain.swapchainImages.size(); ++i)
    {
        if (!createVkImageView(
                vulkanDevice.logicalDevice,
                vulkanSwapchain.swapchainImages[i],
                vulkanSwapchain.imageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
                vulkanSwapchain.swapchainImageViews[i]))
        {
            return false;
        }
    }

    return true;
}

VkSurfaceFormatKHR selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
{
    // // If the list contains only one entry with undefined format
    // // it means that there are no preferred surface formats and any can be
    // // chosen
    // if ((surface_formats.size() == 1) &&
    //     (surface_formats[0].format == VK_FORMAT_UNDEFINED))
    // {
    //     return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR
    //     };
    // }
    //
    // // Check if list contains most widely used R8 G8 B8 A8 format
    // // with nonlinear color space
    // for (VkSurfaceFormatKHR& surface_format : surface_formats)
    // {
    //     if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM)
    //     {
    //         return surface_format;
    //     }
    // }
    //
    // // Return the first format from the list
    // return surface_formats[0];
    for (const auto& surfaceFormat : surfaceFormats)
    {
        // If the list contains only one entry with undefined format
        // it means that there are no preferred surface formats and any can
        // be chosen
        if (surfaceFormat.format ==
                VK_FORMAT_R8G8B8A8_SRGB && // VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR selectPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes)
{
    if (!VSYNC)
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    for (const auto& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    GLFWwindow* window)
{
    // when this is true then vulkan tells us(by setting currentExten)
    // that we have to use this(match the resolution of the window)
    // *** Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within
    // defined confines
    if (surfaceCapabilities.currentExtent.width !=
        std::numeric_limits<std::uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }
    // if false then it tells us that this window manager allow us to differ
    else
    {
        // WIDTH and HEIGH are set in GLFW screen coordinates
        // which might not corresponde to pixel coordinates due to higher
        // pixel density
        // in this case Vulkan didnt fixed the swap extend for us
        int width, height;
        // to query the resolution of the window in pixel before matching it
        // against the minimum and maximum image extent
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { static_cast<std::uint32_t>(width),
                                    static_cast<std::uint32_t>(height) };

        actualExtent.width = std::clamp(
            actualExtent.width,
            surfaceCapabilities.minImageExtent.width,
            surfaceCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
            actualExtent.height,
            surfaceCapabilities.minImageExtent.height,
            surfaceCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}
}
