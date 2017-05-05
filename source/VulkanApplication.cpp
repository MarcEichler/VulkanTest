/*
 * VulkanApplication.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanApplication.h>

#include <assert.h>

std::unique_ptr<VulkanApplication> VulkanApplication::instance;
std::once_flag VulkanApplication::created;

extern vector<const char*> instanceExtensionNames;
extern vector<const char*> layerNames;
extern vector<const char*> deviceExtensionNames;

VulkanApplication::VulkanApplication() {
	// At application startup, enumerate instance layers
	vulkanInstance.layerExtension.getInstanceLayerProperties();

	device = nullptr;

	debugFlag = true;
}

VulkanApplication::~VulkanApplication() {

}

VulkanApplication* VulkanApplication::GetInstance() {
	std::call_once(created, [](){VulkanApplication::instance.reset(new VulkanApplication());});
	return instance.get();
}

void VulkanApplication::initialize() {
	const char* title = "Hello World!";

	// Check if the provided layers are supported
	if (debugFlag) {
		vulkanInstance.layerExtension.areLayersSupported(layerNames);
	}

	// Create the Vulkan instance with the specified layer and extension names
	createVulkanInstance(layerNames, instanceExtensionNames, title);

	// Create the debugging report
	if (debugFlag) {
		vulkanInstance.layerExtension.createDebugReportCallback();
	}

	// Get the list of physical devices
	vector<VkPhysicalDevice> gpuList;
	enumeratePhysicalDevices(gpuList);

	// Handshake with first device
	if (gpuList.size() > 0) {
		handShakeWithDevice(&gpuList[0], layerNames, deviceExtensionNames);
	}
}

void VulkanApplication::deInitialize() {
	this->device->destroyDevice();
	this->vulkanInstance.destroyInstance();

	if (debugFlag) {
		vulkanInstance.layerExtension.destroyDebugReportCallback();
	}
}

VkResult VulkanApplication::createVulkanInstance(vector<const char*>& layers,
		vector<const char*>& extensions, const char* applicationName) {
	return vulkanInstance.createInstance(layers, extensions, applicationName);
}

/**
 * High level function for creating device and queues
 *
 * This function is responsible for creating the logical device.
 * The process of logical device creation requires the following steps:-
 * 1. Get the physical device specific layer and corresponding extensions [Optional]
 * 2. Create user define VulkanDevice object
 * 3. Provide the list of layer and extension that needs to enabled in this physical device
 * 4. Get the physical device or GPU properties
 * 5. Get the memory properties from the physical device or GPU
 * 6. Query the physical device exposed queues and related properties
 * 7. Get the handle of graphics queue
 * 8. Create the logical device, connect it to the graphics queue.
 */
VkResult VulkanApplication::handShakeWithDevice(VkPhysicalDevice* gpu,
		std::vector<const char *>& layers, std::vector<const char *>& extensions) {
	// VulkanDevice to manage the device and its queues
	device = new VulkanDevice(gpu);
	if (!device) return VK_ERROR_OUT_OF_HOST_MEMORY;

	VkResult result;

	// Print available devices
	device->layerExtension.getDeviceExtensionProperties(gpu);

	// Get GPU properties
	vkGetPhysicalDeviceProperties(*gpu, &device->properties);

	// Get GPU memory properties
	vkGetPhysicalDeviceMemoryProperties(*gpu, &device->memoryProperties);

	// Query available queues and their properties
	device->getPhysicalDeviceQueueAndProperties();

	// Retrieve the graphics queue
	device->getGraphicsQueueHandle();

	return device->createDevice(layers, extensions);
}

void VulkanApplication::prepare() {
	// TODO
}

void VulkanApplication::update() {
	// TODO
}

bool VulkanApplication::render() {
	// TODO
	return false;
}

VkResult VulkanApplication::enumeratePhysicalDevices(
		vector<VkPhysicalDevice>& gpuList) {

	uint32_t gpuDeviceCount;

	VkResult result = vkEnumeratePhysicalDevices(vulkanInstance.instance, &gpuDeviceCount, nullptr);
	assert(result == VK_SUCCESS);
	assert(gpuDeviceCount);

	gpuList.resize(gpuDeviceCount);

	result = vkEnumeratePhysicalDevices(vulkanInstance.instance, &gpuDeviceCount, gpuList.data());
	assert(result == VK_SUCCESS);

	return result;
}
