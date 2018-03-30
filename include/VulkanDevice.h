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

/**
 * Represents a logical Vulkan device.
 */
class VulkanDevice {
public:
	VulkanDevice(VkPhysicalDevice* gpu);
	virtual ~VulkanDevice();

public:
	/// Logical Device
	VkDevice	device;

	/// Physical Device
	VkPhysicalDevice* gpu;

	/// Physical device properties
	VkPhysicalDeviceProperties properties;

	/// Physical device memory properties
	VkPhysicalDeviceMemoryProperties memoryProperties;

	/// Device specific extensions
	VulkanLayerAndExtension layerExtension;

public:
	/// Vulkan graphics queue
	VkQueue queue;

	/// Queue family properties
	vector<VkQueueFamilyProperties> queueFamilyProperties;

	/// Stores graphics queue index
	uint32_t graphicsQueueFamilyIndex;

	/// Number of queue family exposed by the device
	uint32_t graphicsQueueWithPresentIndex;

	/// Number of queue families exposed by device
	uint32_t queueFamilyCount;

public:
	/**
	 * Creates the device
	 * @param layers
	 * @param extensions
	 * @return
	 */
	VkResult createDevice(std::vector<const char *>& layers, std::vector<const char *>& extensions);

	/**
	 * Destroys the device
	 */
	void destroyDevice();

	/**
	 * TODO
	 * @param typeBits
	 * @param requirements_mask
	 * @param typeIndex
	 * @return
	 */
	bool memoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);

	/**
	 * TODO
	 */
	void getPhysicalDeviceQueueAndProperties();

	/**
	 * TODO
	 * @return
	 */
	uint32_t getGraphicsQueueHandle();

	/**
	 * TODO
	 */
	void getDeviceQueue();
};

#endif /* INCLUDE_VULKANDEVICE_H_ */
