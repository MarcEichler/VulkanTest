/*
 * VulkanInstance.h
 *
 *  Created on: Mar 1, 2017
 *      Author: marc
 */

#ifndef INCLUDE_VULKANINSTANCE_H_
#define INCLUDE_VULKANINSTANCE_H_

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanLayerAndExtension.h"
#include "VulkanInstance.h"

using namespace std;

class VulkanInstance {
public:
	VulkanInstance();
	virtual ~VulkanInstance();

	// Vulkan instance object
	VkInstance instance;

	// Vulkan specific specific layer and extension
	VulkanLayerAndExtension layerExtension;

	// Functions for creating and deleting the vulkan instance
	VkResult createInstance(vector<const char*>& layers,
							vector<const char*>& extensions,
							const char* applicationName);

	// Destroy Vulkan instance
	void destroyInstance();
};

#endif /* INCLUDE_VULKANINSTANCE_H_ */
