//
// Created by marc on 5/6/17.
//

#ifndef PENGUIN_VULKANSWAPCHAIN_H
#define PENGUIN_VULKANSWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanRenderer.h"
#include "VulkanApplication.h"

class VulkanApplication;
class VulkanRenderer;

/**
 * Contains the image and image view
 */
struct SwapChainBuffer {
    VkImage image;
    VkImageView view;
};

struct SwapChainPrivateVariables {
    /// The images' surface capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities;

    /// The number of present mode supported by the implementation
    uint32_t presentModeCount;

    /// The retrieved present modes
    std::vector<VkPresentModeKHR> presentModes;

    /// Size of the swap chain color images
    VkExtent2D swapChainExtent;

    /// Number of color images supported by the implementation
    uint32_t desiredNumberOfSwapChainImages;

    VkSurfaceTransformFlagBitsKHR preTransform;

    /// The present mode bitwise flag for the creation of the swap chain
    VkPresentModeKHR swapChainPresentMode;

    /// The retrieved drawing color swap chain images
    std::vector<VkImage> swapChainImages;

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
};

struct SwapChainPublicVariables {
    /// The logical platform dependent surface object
    VkSurfaceKHR surface;

    /// Number of buffer images used for the swap chain
    uint32_t swapchainImageCount;

    /// Swap chain object
    VkSwapchainKHR swapChain;

    /// List of color swap chain images
    std::vector<struct SwapChainBuffer> colorBuffer;

    // Semaphore for sync purpose
    VkSemaphore  presentCompleteSemaphore;

    /// Current drawing surface index in use
    uint32_t currentColorBuffer;

    /// Image format
    VkFormat format;
};

/**
 * \brief Manages a Vulkan swap chain.
 */
class VulkanSwapChain {
public:
    /**
     * \brief Constructor
     * @param renderer
     */
    VulkanSwapChain(VulkanRenderer* renderer);

    /**
     * \brief Destructor
     */
    ~VulkanSwapChain();

    /**
     * \brief Initializes the swap chain
     */
    void initializeSwapChain();

    /**
     * Creates the swap chain
     * @param cmd
     */
    void createSwapChain(const VkCommandBuffer& cmd);

    /**
     * Destroys the swap chain
     */
    void destroySwapChain();

private:
    VkResult createSwapChainExtensions();
    void getSupportedFormats();
    VkResult createSurface();

    /**
     * Returns the handle to a queue that supports presentation requrests.
     * @return
     */
    uint32_t getGraphicsQueueWithPresentationSupport();
    void getSurfaceCapabilitiesAndPresentMode();

    /**
     * Manages the presentation mode
     */
    void managePresentMode();

    /**
     * Creates the swapchains' color images
     */
    void createSwapChainColorImages();
    void createColorImageView(const VkCommandBuffer& cmd);

public:
    struct SwapChainPublicVariables publicVariables;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;

private:
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR        fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR   fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR        fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR   fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkDestroySurfaceKHR                         fpDestroySurfaceKHR;

    // Layer Extensions Debugging
    PFN_vkCreateSwapchainKHR    fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR   fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;

    struct SwapChainPrivateVariables privateVariables;
    VulkanRenderer* renderer;
    VulkanApplication* application;
};


#endif //PENGUIN_VULKANSWAPCHAIN_H
