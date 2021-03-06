/*
* Learning Vulkan - ISBN: 9781786469809
*
* Author: Parminder Singh, parminder.vulkan@gmail.com
* Linkedin: https://www.linkedin.com/in/parmindersingh18
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include "VulkanRenderer.h"
#include "Wrappers.h"

#include <assert.h>
#include <cstring>
#include <iostream>
#include <Header.h>

VulkanRenderer::VulkanRenderer(VulkanApplication * application, VulkanDevice* device)
{
    assert(application != nullptr);
    assert(device != nullptr);

    // Note: It's very important to initialize the member with 0 or respective value otherwise it will break the system
    memset(&Depth, 0, sizeof(Depth));
#ifdef _WIN32
    memset(&connection, 0, sizeof(HINSTANCE));				// hInstance - Windows Instance
#endif

    this->application = application;
    this->device	= device;

    swapchain = new VulkanSwapChain(this);
}

VulkanRenderer::~VulkanRenderer()
{
    delete swapchain;
    swapchain = nullptr;
}

void VulkanRenderer::initialize()
{
    // Create an empty window 500x500
    createPresentationWindow(500, 500);

    // Initialize swapchain
    swapchain->initializeSwapChain();

    // We need command buffers, so create a command buffer pool
    createCommandPool();

    // Let's create the swap chain color images and depth image
    buildSwapChainAndDepthImage();
}

#ifdef _WIN32
bool VulkanRenderer::render()
{
    MSG msg;   // message
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
    if (msg.message == WM_QUIT) {
        return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    RedrawWindow(window, NULL, NULL, RDW_INTERNALPAINT);
    return true;
}

// MS-Windows event handling function:
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VulkanApplication* appObj = VulkanApplication::GetInstance();
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int& windowWidth, const int& windowHeight)
{
	width	= windowWidth;
	height	= windowHeight;
	assert(width > 0 || height > 0);

	WNDCLASSEX  winInfo;

	sprintf(name, "Swapchain presentation window");
	memset(&winInfo, 0, sizeof(WNDCLASSEX));
	// Initialize the window class structure:
	winInfo.cbSize			= sizeof(WNDCLASSEX);
	winInfo.style			= CS_HREDRAW | CS_VREDRAW;
	winInfo.lpfnWndProc		= WndProc;
	winInfo.cbClsExtra		= 0;
	winInfo.cbWndExtra		= 0;
	winInfo.hInstance		= connection; // hInstance
	winInfo.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winInfo.hCursor			= LoadCursor(NULL, IDC_ARROW);
	winInfo.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	winInfo.lpszMenuName	= NULL;
	winInfo.lpszClassName	= name;
	winInfo.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);

	// Register window class:
	if (!RegisterClassEx(&winInfo)) {
		// It didn't work, so try to give a useful error:
		printf("Unexpected error trying to start the application!\n");
		fflush(stdout);
		exit(1);
	}

	// Create window with the registered class:
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	window = CreateWindowEx(0,
							name,					// class name
							name,					// app name
							WS_OVERLAPPEDWINDOW |	// window style
							WS_VISIBLE |
							WS_SYSMENU,
							100, 100,				// x/y coords
							wr.right - wr.left,     // width
							wr.bottom - wr.top,     // height
							NULL,					// handle to parent
							NULL,					// handle to menu
							connection,				// hInstance
							NULL);					// no extra parameters

	if (!window) {
		// It didn't work, so try to give a useful error:
		printf("Cannot create a window in which to draw!\n");
		fflush(stdout);
		exit(1);
	}

	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)&application);
}

void VulkanRenderer::destroyPresentationWindow()
{
	DestroyWindow(window);
}
#else
void VulkanRenderer::createPresentationWindow(const int& width, const int& height) {
    this->width = width;
    this->height = height;

    assert(width > 0);
    assert(height > 0);

    DEBUG("VulkanRenderer >> Creating window");

    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(NULL, &scr);
    if (connection == NULL) {
        ERROR("Cannot find a compatible Vulkan ICD.\n");
        exit(-1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;

    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, (uint16_t) width, (uint16_t) height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

    // Magic code that will send notification when window is destroyed
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    reply = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1,	&(*reply).atom);
    free(reply);

    xcb_map_window(connection, window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive runs
    const uint32_t coords[] = { 100,  100 };
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
    }
}

bool VulkanRenderer::render() {
    bool running = true;
    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    while (running) {
        event = xcb_wait_for_event(connection);
        if (!event) {
            running = false;
            break;
        }

        switch (event->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                cm = (xcb_client_message_event_t *)event;

                if (cm->data.data32[0] == reply->atom)
                    running = false;

                break;
            }
        }

        free(event);
    }
    return false;
}

void VulkanRenderer::destroyPresentationWindow() {
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
}

#endif // _WIN32

void VulkanRenderer::createCommandPool() {
    VulkanDevice* device = application->device;
    /* Depends on intializeSwapChainExtension() */
    VkResult  res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = NULL;
    cmdPoolInfo.queueFamilyIndex = device->graphicsQueueWithPresentIndex;
    cmdPoolInfo.flags = 0;

    res = vkCreateCommandPool(device->device, &cmdPoolInfo, NULL, &cmdPool);
    assert(res == VK_SUCCESS);
}

void VulkanRenderer::buildSwapChainAndDepthImage() {
    // Get the appropriate queue to submit the command into
    device->getDeviceQueue();

    // Create swapchain and get the color image
    swapchain->createSwapChain(cmdDepthImage);

    // Create the depth image
    createDepthImage();
}

void VulkanRenderer::createDepthImage() {
    VkResult  result;
    bool  pass;

    // If the depth format is undefined, use fallback as 16-byte value
    if (Depth.format == VK_FORMAT_UNDEFINED) {
        Depth.format = VK_FORMAT_D16_UNORM;
    }

    const VkFormat depthFormat = Depth.format;

    VkImageCreateInfo imageInfo = {};

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(*device->gpu, depthFormat, &props);
    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    }
    else if (props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
    }
    else {
        std::cout << "Unsupported Depth Format, try other Depth formats.\n";
        exit(-1);
    }

    imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext			= nullptr;
    imageInfo.imageType		= VK_IMAGE_TYPE_2D;
    imageInfo.format		= depthFormat;
    imageInfo.extent.width	= (uint32_t) width;
    imageInfo.extent.height = (uint32_t) height;
    imageInfo.extent.depth	= 1;
    imageInfo.mipLevels		= 1;
    imageInfo.arrayLayers	= 1;
    imageInfo.samples		= NUM_SAMPLES;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices	= nullptr;
    imageInfo.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.usage					= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.flags					= 0;

    result = vkCreateImage(device->device, &imageInfo, nullptr, &Depth.image);
    assert(result == VK_SUCCESS);

    // Get the image memory requirements
    VkMemoryRequirements memRqrmnt;
    vkGetImageMemoryRequirements(device->device,	Depth.image, &memRqrmnt);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;
    // Determine the type of memory required with the help of memory properties
    pass = device->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, /* No requirements */ &memAlloc.memoryTypeIndex);
    assert(pass);

    // Allocate the memory
    result = vkAllocateMemory(device->device, &memAlloc, NULL, &Depth.mem);
    assert(result == VK_SUCCESS);

    // Bind the allocated memory
    result = vkBindImageMemory(device->device, Depth.image, Depth.mem, 0);
    assert(result == VK_SUCCESS);

    // Create image view
    VkImageViewCreateInfo imgViewInfo = {};
    imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.pNext = nullptr;
    imgViewInfo.image = VK_NULL_HANDLE;
    imgViewInfo.format = depthFormat;
    imgViewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
    imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imgViewInfo.subresourceRange.baseMipLevel = 0;
    imgViewInfo.subresourceRange.levelCount = 1;
    imgViewInfo.subresourceRange.baseArrayLayer = 0;
    imgViewInfo.subresourceRange.layerCount = 1;
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    // Use command buffer to create the depth image. This includes -
    // Command buffer allocation, recording with begin/end scope and submission.
    CommandBufferMgr::allocCommandBuffer(&device->device, cmdPool, &cmdDepthImage);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);

    // Set the image layout to depth stencil optimal
    setImageLayout(Depth.image,
                   imgViewInfo.subresourceRange.aspectMask,
                   VK_IMAGE_LAYOUT_UNDEFINED,
                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, (VkAccessFlagBits)0, cmdDepthImage);

    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(device->queue, &cmdDepthImage);

    // Create the image view and allow the application to use the images.
    imgViewInfo.image = Depth.image;
    result = vkCreateImageView(device->device, &imgViewInfo, NULL, &Depth.view);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::setImageLayout(VkImage image,
                                    VkImageAspectFlags aspectMask,
                                    VkImageLayout oldImageLayout,
                                    VkImageLayout newImageLayout,
                                    VkAccessFlagBits srcAccessMask,
                                    const VkCommandBuffer& cmd) {
    // Dependency on cmd
    assert(cmd != VK_NULL_HANDLE);

    // The device->queue must be initialized
    assert(device->queue != VK_NULL_HANDLE);

    VkImageMemoryBarrier imgMemoryBarrier = {};
    imgMemoryBarrier.sType			= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgMemoryBarrier.pNext			= NULL;
    imgMemoryBarrier.srcAccessMask	= srcAccessMask;
    imgMemoryBarrier.dstAccessMask	= 0;
    imgMemoryBarrier.oldLayout		= oldImageLayout;
    imgMemoryBarrier.newLayout		= newImageLayout;
    imgMemoryBarrier.image			= image;
    imgMemoryBarrier.subresourceRange.aspectMask	= aspectMask;
    imgMemoryBarrier.subresourceRange.baseMipLevel	= 0;
    imgMemoryBarrier.subresourceRange.levelCount	= 1;
    imgMemoryBarrier.subresourceRange.layerCount	= 1;

    if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    switch (newImageLayout) {
        // Ensure that anything that was copying from this image has completed
        // An image in this layout can only be used as the destination operand of the commands
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            break;

        // Ensure any Copy or CPU writes to image are flushed
        // An image in this layout can only be used as a read-only shader resource
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        // An image in this layout can only be used as a framebuffer color attachment
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            break;

        // An image in this layout can only be used as a framebuffer depth/stencil attachment
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        default: assert(false);
    }

    VkPipelineStageFlags srcStages	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(cmd, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}

void VulkanRenderer::destroyDepthBuffer() {
    vkDestroyImageView(device->device, Depth.view, NULL);
    vkDestroyImage(device->device, Depth.image, NULL);
    vkFreeMemory(device->device, Depth.mem, NULL);
}

void VulkanRenderer::destroyCommandBuffer() {
    VkCommandBuffer cmdBufs[] = { cmdDepthImage };
    vkFreeCommandBuffers(device->device, cmdPool, sizeof(cmdBufs)/sizeof(VkCommandBuffer), cmdBufs);
}

void VulkanRenderer::destroyCommandPool() {
    VulkanDevice* deviceObj		= application->device;

    vkDestroyCommandPool(deviceObj->device, cmdPool, NULL);
}
