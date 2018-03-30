/*
 * main.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: Marc Eichler
 */

#include <vulkan/vulkan.h>
#include <cstdint>
#include <iostream>
#include <vector>

#include <X11/Xlib.h>

#include "VulkanApplication.h"

using namespace std;

vector<const char *> instanceExtensionNames = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XCB_KHR)
		VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
};

vector<const char *> layerNames = {
		"VK_LAYER_LUNARG_api_dump"
};

std::vector<const char *> deviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int main (int argc, char **argv) {
	cout << "*** VulkanTest started! ***\n";

	VulkanApplication* app = VulkanApplication::GetInstance();
	app->initialize();
    app->prepare();
    app->render();
    app->deInitialize();


	cout << "*** VulkanTest ended successfully! ***\n";
	return 0;
}
