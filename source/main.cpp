/*
 * main.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: marc
 */

#include <cstdint>
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

using namespace std;

int main (int argc, char **argv) {
	cout << "*** VulkanTest started! ***\n";

	/*** 1. Enumerate Instance Layer properties ***/
	// Get number of instance layers
	uint32_t instanceLayerCount;

	// Use second parameter as NULL to return the layer count
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

	VkLayerProperties *layerProperty = nullptr;

	vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperty);

	// Get the extensions for each available instance layer
	/*foreach  layerProperty{
	        VkExtensionProperties *instanceExtensions;
	        res = vkEnumerateInstanceExtensionProperties(layer_name,
	                        &instanceExtensionCount, instanceExtensions);
	}*/

	/*** 2. Instance Creation ***/
	// Vulkan instance object
	VkInstance instance;
	VkInstanceCreateInfo instanceInfo        = {};

	// Specify layer and extensions names that needs to be enabled on instance.
	/*instanceInfo.ppEnabledLayerNames         = {"VK_LAYER_LUNARG_standard_validation",
	                                                                                "VK_LAYER_LUNARG_object_tracker" };*/

	vector<const char *> instanceExtensions = {VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XCB_KHR)
			VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
};

	// Specify extensions that needs to be enabled on instance.
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

	vkCreateInstance(&instanceInfo, nullptr, &instance);

	/*** 3. Enumerate physical devices ***/
	// Physical device count
	uint32_t	gpuCount;

	// Get number of GPUs
	vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);

	// List of physical devices
	std::vector<VkPhysicalDevice> gpuList(gpuCount);

	// Get GPU Information
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpuList.data());

	// Print devices
	cout << "Physical devices: " << gpuCount << endl;
	if (gpuCount < 1) {
		cout << "No physical devices found!" << endl;
		return -1;
	}
	for (uint32_t i = 0; i < gpuCount; i++) {
		// Get the physical device or GPU properties
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(gpuList[i], &properties);
		cout << "*** Device" << i << "***" << endl;
		cout << "Device Name\t"; std::cout.write(properties.deviceName, sizeof(properties.deviceName) - 1); cout << endl;
		cout << "Vendor ID\t" << properties.vendorID << endl;
		cout << "Device ID\t" << properties.deviceID << endl;
		cout << "API Version\t" << properties.apiVersion << endl;
		cout << "Driver Version\t" << properties.driverVersion << endl;
		cout << "Device Type\t" << properties.deviceType << endl;
		cout << endl;
	}

	/*** 4. Create Device ***/
	// Get Queue and Queue Type
	//vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueCount, queueProperties);

	// Get the memory properties from the physical device or GPU
	//vkGetPhysicalDeviceMemoryProperties(gpu, memoryProperties)
	VkDeviceCreateInfo deviceInfo = {};
	VkDevice device;
	vkCreateDevice(gpuList[0], &deviceInfo, nullptr, &device);
	cout << "Retrieved logical device" << endl;

	cout << "*** VulkanTest ended successfully! ***\n";
	return 0;
}


