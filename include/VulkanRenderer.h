#pragma once

#include "VulkanSwapChain.h"

class VulkanApplication;
class VulkanDevice;
class VulkanSwapChain;

/* Number of samples needs to be the same at image creation
 * Used at renderpass creation (in attachment) and pipeline creation
 */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

/**
 * This class manages platform specific presentation window and its drawing surface.
 */
class VulkanRenderer {
public:
    /**
     * Constructor
     * @param app
     * @param deviceObject
     */
    VulkanRenderer(VulkanApplication* app, VulkanDevice* deviceObject);

    /*
     * Destructor
     */
    ~VulkanRenderer();

public:
    /**
     * Initializes the presentation window
     */
    void initialize();

    /**
     * Renders the presentation window.
     * @return
     */
    bool render();

    /**
     * Create an empty window.
     * @param windowWidth
     * @param windowHeight
     */
    void createPresentationWindow(const int& windowWidth = 500, const int& windowHeight = 500);

    /**
     * TODO
     * @param image
     * @param aspectMask
     * @param oldImageLayout
     * @param newImageLayout
     * @param srcAccessMask
     * @param cmdBuf
     */
    void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer& cmdBuf);

#ifdef _WIN32
    /**
     * Windows procedure method for handling events.
     */
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

    /**
     * \brief Destroys the presentation window.
     */
    void destroyPresentationWindow();

    /**
     * \brief Returns the VulkanApplication.
     * @return the VulkanApplication
     */
    inline VulkanApplication* getApplication()		{ return application; }

    /**
     * \brief Returns the device.
     * @return the device
     */
    inline VulkanDevice*  getDevice()				{ return device; }

    /**
     * \brief Returns the swap chain.
     * @return the swap chain
     */
    inline VulkanSwapChain*  getSwapChain() 		{ return swapchain; }

    /**
     * \brief Creates the command pool.
     */
    void createCommandPool();

    /**
     * \brief Creates the swapchain color image and depth image
     */
    void buildSwapChainAndDepthImage();

    /**
     * \brief Creates the depth image
     */
    void createDepthImage();

    /**
     * \brief Destroys the command buffer.
     */
    void destroyCommandBuffer();

    /**
     * \brief Destroys the command pool.
     */
    void destroyCommandPool();

    /**
     * \brief Destroys the depth buffer
     */
    void destroyDepthBuffer();

public:
#ifdef _WIN32
    #define APP_NAME_STR_LEN 80
    /**
     * Windows hInstance
     */
    HINSTANCE					connection;

    /**
     * The name appearing on the application window
     */
    char						name[APP_NAME_STR_LEN];

    /**
     * The window handle hWnd
     */
    HWND						window;
#else
    /**
     * The X connection
     */
    xcb_connection_t*			connection;

    /**
     * The X screen
     */
    xcb_screen_t*				screen;

    /**
     * The X window
     */
    xcb_window_t				window;

    /**
     * The X reply
     */
    xcb_intern_atom_reply_t*	reply;
#endif

    struct{
        VkFormat		format;
        VkImage			image;
        VkDeviceMemory	mem;
        VkImageView		view;
    } Depth;

    /**
     * Command buffer for the depth image layout
     */
    VkCommandBuffer		cmdDepthImage;

    /**
     * The command pool
     */
    VkCommandPool		cmdPool;

    /**
     * The window width
     */
    int width;

    /**
     * The window height
     */
    int height;

private:
    /**
     * The VulkanApplication
     */
    VulkanApplication* application;
    /*
     * The device object associated with this Presentation layer.
     */
    VulkanDevice*	   device;

    /**
     * The swapchain
     */
    VulkanSwapChain* swapchain;
};