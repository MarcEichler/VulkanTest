/*
 * VulkanDevice.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanDevice.h>

#include <assert.h>
#include <iostream>
#include <Header.h>

VulkanDevice::VulkanDevice(VkPhysicalDevice* gpu) {
	this->gpu = gpu;
}

VulkanDevice::~VulkanDevice() {
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
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
	VkDeviceQueueCreateInfo queueInfo   = {};
	queueInfo.queueFamilyIndex          = graphicsQueueFamilyIndex;
	queueInfo.sType                     = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext                     = nullptr;
	queueInfo.pQueuePriorities          = queuePriorities;
    queueInfo.queueCount                = 1;

	VkDeviceCreateInfo deviceInfo       = {};
	deviceInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pNext                    = nullptr;
	deviceInfo.queueCreateInfoCount     = 1;
	deviceInfo.pQueueCreateInfos        = &queueInfo;
	deviceInfo.enabledLayerCount        = 0;
	deviceInfo.ppEnabledLayerNames      = nullptr;
	deviceInfo.enabledExtensionCount    = (uint32_t) extensions.size();
	deviceInfo.ppEnabledExtensionNames  = (extensions.size()) ? extensions.data(): nullptr;
	deviceInfo.pEnabledFeatures         = nullptr;

	result = vkCreateDevice(*gpu, &deviceInfo, nullptr, &device);

	INFO("*** Created logical device");

	assert(result == VK_SUCCESS);
	return result;
}
#pragma clang diagnostic pop

void VulkanDevice::destroyDevice() {
	vkDestroyDevice(this->device, nullptr);
}

void VulkanDevice::getPhysicalDeviceQueueAndProperties() {
	// Query queue family count
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, nullptr);

	queueFamilyProperties.resize(queueFamilyCount);

	// Get queue family properties
	vkGetPhysicalDeviceQueueFamilyProperties(*gpu, &queueFamilyCount, queueFamilyProperties.data());

	INFO("\t*** Available queue families:");
	int i = 0;
	for (auto prop : queueFamilyProperties) {
		INFO("\t" << i << ":");
		INFO("\t\tGraphics: " << ((prop.queueFlags&VK_QUEUE_GRAPHICS_BIT)?"enabled":"disabled"));
		INFO("\t\tCompute: " << ((prop.queueFlags&VK_QUEUE_COMPUTE_BIT)?"enabled":"disabled"));
		INFO("\t\tTransfer: " << ((prop.queueFlags&VK_QUEUE_TRANSFER_BIT)?"enabled":"disabled"));
		INFO("\t\tSparse: " << ((prop.queueFlags&VK_QUEUE_SPARSE_BINDING_BIT)?"enabled":"disabled"));
		INFO("\t\tQueues: " << prop.queueCount);
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

bool VulkanDevice::memoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex) {
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < 32; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

void VulkanDevice::getDeviceQueue() {
    vkGetDeviceQueue(device, graphicsQueueWithPresentIndex, 0, &queue);
}