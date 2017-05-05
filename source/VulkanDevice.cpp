/*
 * VulkanDevice.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanDevice.h>

#include <assert.h>
#include <iostream>

VulkanDevice::VulkanDevice(VkPhysicalDevice* gpu) {
	this->gpu = gpu;

	this->queueFamilyCount = 0;
	this->graphicsQueueFamilyIndex = 0;
	this->queue = VkQueue();
	this->device = nullptr;
}

VulkanDevice::~VulkanDevice() {
}

/**
 * Creates the VkDevice.
 *
 * Note: Requires the queue information to be in place before being called.
 */
VkResult VulkanDevice::createDevice(std::vector<const char*>& layers,
		std::vector<const char*>& extensions) {
	VkResult result;

	float queuePriorities[1] = { 0.0 };

	// Create object information
	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext = nullptr;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledLayerNames = nullptr;
	deviceInfo.enabledExtensionCount = extensions.size();
	deviceInfo.ppEnabledExtensionNames = extensions.data();
	deviceInfo.pEnabledFeatures = nullptr;

	result = vkCreateDevice(*gpu, &deviceInfo, nullptr, &device);

	cout << "*** Created logical device" << endl;

	assert(result == VK_SUCCESS);
	return result;
}

void VulkanDevice::destroyDevice() {
	vkDestroyDevice(this->device, nullptr);
}

void VulkanDevice::getPhysicalDeviceQueueAndProperties() {
	// Query queue family count
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, nullptr);

	queueFamilyProperties.resize(queueFamilyCount);

	// Get queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProperties.data());

	cout << "\t*** Available queue families:" << endl;
	int i = 0;
	for (auto prop : queueFamilyProperties) {
		cout << "\t" << i << ":" << endl;
		cout << "\t\tGraphics: " << ((prop.queueFlags&VK_QUEUE_GRAPHICS_BIT)?"enabled":"disabled") << endl;
		cout << "\t\tCompute: " << ((prop.queueFlags&VK_QUEUE_COMPUTE_BIT)?"enabled":"disabled") << endl;
		cout << "\t\tTransfer: " << ((prop.queueFlags&VK_QUEUE_TRANSFER_BIT)?"enabled":"disabled") << endl;
		cout << "\t\tSparse: " << ((prop.queueFlags&VK_QUEUE_SPARSE_BINDING_BIT)?"enabled":"disabled") << endl;
		cout << "\t\tQueues: " << prop.queueCount << endl;
		i++;
	}
}

uint32_t VulkanDevice::getGraphicsQueueHandle() {
	bool found = false;

	// Iterate number of supported queue families
	for (uint32_t i = 0; i < queueFamilyCount; ++i) {
		// Check if graphics queue
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			found = true;
			graphicsQueueFamilyIndex = i;
			break;
		}
	}

	assert(found);

	return 0;
}


