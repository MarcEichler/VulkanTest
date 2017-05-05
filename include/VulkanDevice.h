/*
 * VulkanDevice.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#ifndef INCLUDE_VULKANDEVICE_H_
#define INCLUDE_VULKANDEVICE_H_

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanLayerAndExtension.h"

using namespace std;

class VulkanDevice {
public:
	VulkanDevice(VkPhysicalDevice* gpu);
	virtual ~VulkanDevice();

	VkResult createDevice(std::vector<const char *>& layers, std::vector<const char *>& extensions);
	void destroyDevice();

	void getPhysicalDeviceQueueAndProperties();

	uint32_t getGraphicsQueueHandle();

	/**
	 * Logical Device
	 */
	VkDevice	device;

	/**
	 * Physical Device
	 */
	VkPhysicalDevice* gpu;

	/**
	 * Physical device properties
	 */
	VkPhysicalDeviceProperties properties;

	/**
	 * Physical device memory properties
	 */
	VkPhysicalDeviceMemoryProperties memoryProperties;

	/**
	 * Vulkan graphics queue
	 */
	VkQueue queue;

	/**
	 * Queue family properties
	 */
	vector<VkQueueFamilyProperties> queueFamilyProperties;

	/**
	 * Stores graphics queue index
	 */
	uint32_t graphicsQueueFamilyIndex;

	/**
	 * Number of queue families exposed by device
	 */
	uint32_t queueFamilyCount;

	/**
	 * Device specific extensions
	 */
	VulkanLayerAndExtension layerExtension;
};

#endif /* INCLUDE_VULKANDEVICE_H_ */
