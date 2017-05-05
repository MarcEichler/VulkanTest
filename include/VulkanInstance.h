/*
 * VulkanInstance.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#ifndef INCLUDE_VULKANINSTANCE_H_
#define INCLUDE_VULKANINSTANCE_H_

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanLayerAndExtension.h"
#include "VulkanInstance.h"

using namespace std;

/**
 * Wrapper class for the Vulkan instance (vkInstance)
 */
class VulkanInstance {
public:
	VulkanInstance();
	virtual ~VulkanInstance();

	/**
	 * The Vulkan instance
	 */
	VkInstance instance;

	/**
	 * The Vulkan specific layers and extensions
	 */
	VulkanLayerAndExtension layerExtension;

	/**
	 * Creates the Vulkan instance
	 * @param layerNames		The layer names
	 * @param extensions		The extension names
	 * @param applicationName	The application name
	 * @return successful?
	 */
	VkResult createInstance(vector<const char*>& layerNames,
							vector<const char*>& extensions,
							const char* applicationName);

	/**
	 * Destroys the current vulkan instance.
	 */
	void destroyInstance();
};

#endif /* INCLUDE_VULKANINSTANCE_H_ */
