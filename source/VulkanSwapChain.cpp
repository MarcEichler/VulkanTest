//
// Created by marc on 5/6/17.
//

#include <vulkan/vulkan.h>
#include <iostream>
#include <assert.h>

#include "VulkanSwapChain.h"

#define INSTANCE_FUNC_PTR(instance, entrypoint) {                                           \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(instance, "vk"#entrypoint); \
    if (fp##entrypoint == nullptr) {                                                        \
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;             \
        exit(-1);                                                                           \
    }                                                                                       \
}

#define DEVICE_FUNC_PTR(dev, entrypoint){													\
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);		\
    if (fp##entrypoint == nullptr) {															\
        std::cout << "Unable to locate the vkGetDeviceProcAddr: vk"#entrypoint;				\
        exit(-1);																			\
    }																						\
}

VulkanSwapChain::VulkanSwapChain(VulkanRenderer *renderer) {
    this->renderer = renderer;
    this->application = VulkanApplication::GetInstance();
}

VulkanSwapChain::~VulkanSwapChain() {
    privateVariables.swapChainImages.clear();
    privateVariables.surfaceFormats.clear();
    privateVariables.presentModes.clear();
}

VkResult VulkanSwapChain::createSwapChainExtensions() {
    // Dependency on createPresentationWindow()
    VkInstance& instance = application->vulkanInstance.instance;
    VkDevice& device = application->device->device;

    // Get Instance based swapchain extension function pointer
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceSupportKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
    INSTANCE_FUNC_PTR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
    INSTANCE_FUNC_PTR(instance, DestroySurfaceKHR);

    // Get Device based swapchain extension function pointer
    DEVICE_FUNC_PTR(device, CreateSwapchainKHR);
    DEVICE_FUNC_PTR(device, DestroySwapchainKHR);
    DEVICE_FUNC_PTR(device, GetSwapchainImagesKHR);
    DEVICE_FUNC_PTR(device, AcquireNextImageKHR);
    DEVICE_FUNC_PTR(device, QueuePresentKHR);
}

void VulkanSwapChain::initializeSwapChain() {

}

void VulkanSwapChain::createSwapChain(const VkCommandBuffer &cmd) {

}

void VulkanSwapChain::destroySwapChain() {

}

void VulkanSwapChain::getSupportedFormats() {
    const VkPhysicalDevice gpu = *renderer->getDevice()->gpu;
    VkResult result;
    // Get the number of supported VkFormats
    uint32_t formatCount;
    fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, publicVariables.surface, &formatCount, nullptr);
    assert(formatCount > 0);

    privateVariables.surfaceFormats.clear();
    privateVariables.surfaceFormats.reserve(formatCount);

    // Get VkFormats in allocated objects
    result = fpGetPhysicalDeviceSurfaceFormatsKHR(gpu, publicVariables.surface, &formatCount, &privateVariables.surfaceFormats[0]);

    // If the format is VK_FORMAT_UNDEFINED, the surface has no preferred format.
    // Use RGBA 32 bit in this case
    if (formatCount == 1 && privateVariables.surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        publicVariables.format = VK_FORMAT_B8G8R8_UNORM;
    } else {
        publicVariables.format = privateVariables.surfaceFormats[0].format;
    }
}

VkResult VulkanSwapChain::createSurface() {
    return VK_ERROR_INITIALIZATION_FAILED;
}

uint32_t VulkanSwapChain::getGraphicsQueueWithPresentationSupport() {
    const VulkanDevice& device = *application->device;
    uint32_t queueCount = device.queueFamilyCount;
    const VkPhysicalDevice& gpu = *device.gpu;
    const vector<VkQueueFamilyProperties>& queueProps = device.queueFamilyProperties;

    // Iterate each queue and get its extension status
    VkBool32*const supportsPresent = (VkBool32*) malloc(queueCount*sizeof(VkBool32));
    for (uint32_t i = 0; i < queueCount; i++) {
        fpGetPhysicalDeviceSurfaceSupportKHR(gpu, i, publicVariables.surface, &supportsPresent[i]);
    }

    // Search for graphics queues that support presentation
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    // If no queue was found that supports both graphics and present, find a separate present queue
    if (presentQueueNodeIndex == UINT32_MAX) {
        for (uint32_t i = 0; i < queueCount; i++) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    free((void*) supportsPresent);

    // Generate error if no suitable queue was found
    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) return UINT32_MAX;

    return graphicsQueueNodeIndex;
}

void VulkanSwapChain::getSurfaceCapabilitiesAndPresentMode() {
    VkResult result;
    const VkPhysicalDevice gpu = *application->device->gpu;

    result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, publicVariables.surface, &privateVariables.surfaceCapabilities);
    assert(result == VK_SUCCESS);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, publicVariables.surface, &privateVariables.presentModeCount, nullptr);
    assert(result == VK_SUCCESS);

    privateVariables.presentModes.clear();
    privateVariables.presentModes.resize(privateVariables.presentModeCount);
    assert(privateVariables.presentModes.size() >= 1);

    result = fpGetPhysicalDeviceSurfacePresentModesKHR(gpu, publicVariables.surface, &privateVariables.presentModeCount, &privateVariables.presentModes[0]);
    assert(result == VK_SUCCESS);

    if (privateVariables.surfaceCapabilities.currentExtent.width <= (uint32_t) -1) {
        // No width and height defined, use image size
        privateVariables.swapChainExtent.width = (uint32_t) renderer->width;
        privateVariables.swapChainExtent.height = (uint32_t) renderer->height;
    } else {
        // If the surface is defined, it must match the current image size
        privateVariables.swapChainExtent = privateVariables.surfaceCapabilities.currentExtent;
    }
}

void VulkanSwapChain::managePresentMode() {
    // If available, use MAILBOX, otherwise try to use IMMEDIATE.
    // Use FIFO as the fallback
    privateVariables.swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < privateVariables.presentModeCount; i++) {
        if (privateVariables.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            privateVariables.swapChainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        } else if (privateVariables.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            privateVariables.swapChainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use. We try to use 1 + image being displayed + image being queued
    privateVariables.desiredNumberOfSwapChainImages = privateVariables.surfaceCapabilities.minImageCount + 1;
    if ((privateVariables.surfaceCapabilities.maxImageCount > 0) &&
        privateVariables.desiredNumberOfSwapChainImages > privateVariables.surfaceCapabilities.maxImageCount) {
        privateVariables.desiredNumberOfSwapChainImages = privateVariables.surfaceCapabilities.maxImageCount;
    }

    if (privateVariables.surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        privateVariables.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        privateVariables.preTransform = privateVariables.surfaceCapabilities.currentTransform;
    }
}

void VulkanSwapChain::createSwapChainColorImages() {
    // Initialize create info
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.surface = publicVariables.surface;
    createInfo.minImageCount = privateVariables.desiredNumberOfSwapChainImages;
    createInfo.imageFormat = publicVariables.format;
    createInfo.imageExtent.width = privateVariables.swapChainExtent.width;
    createInfo.imageExtent.height = privateVariables.swapChainExtent.height;
    createInfo.preTransform = privateVariables.preTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.presentMode = privateVariables.swapChainPresentMode;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.clipped = true;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;

    // Create the swapchain object
    fpCreateSwapchainKHR(renderer->getDevice()->device, &createInfo, nullptr, &publicVariables.swapChain);

    // Get the number of images
    fpGetSwapchainImagesKHR(renderer->getDevice()->device, publicVariables.swapChain, &publicVariables.swapchainImageCount, nullptr);

    // Create array to retrieve the swapchain images
    privateVariables.swapChainImages.clear();
    privateVariables.swapChainImages.resize(publicVariables.swapchainImageCount);
    assert(privateVariables.swapChainImages.size() >= 1);

    // Retrieve the swapchain image surfaces
    fpGetSwapchainImagesKHR(renderer->getDevice()->device, publicVariables.swapChain, &publicVariables.swapchainImageCount, &privateVariables.swapChainImages[0]);
}

void VulkanSwapChain::createColorImageView(const VkCommandBuffer& cmd) {
    VkResult result;

    for (uint32_t i = 0; i < publicVariables.swapchainImageCount; i++) {
        SwapChainBuffer scBuffer;

        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.format = publicVariables.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 0;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.flags = 0;

        scBuffer.image = privateVariables.swapChainImages[i];

        // Since the swapchain is not owned by us we cannot set the image layout
        // upon setting the implementation may give error, the images layout were
        // create by the WSI implementation not by us.

        createInfo.image = scBuffer.image;

        result = vkCreateImageView(renderer->getDevice()->device, &createInfo, nullptr, &scBuffer.view);
        assert(result == VK_SUCCESS);

        publicVariables.colorBuffer.push_back(scBuffer);
    }
    publicVariables.currentColorBuffer = 0;
}
