/*
 * VulkanLayerAndExtension.h
 *
 *  Created on: Mar 1, 2017
 *      Author: marc
 */

#pragma once

#include <vulkan/vulkan.h>

#include <vector>

using namespace std;

struct LayerProperties {
	VkLayerProperties	properties;
	vector<VkExtensionProperties> extensions;
};

class VulkanLayerAndExtension {
public:
	VulkanLayerAndExtension();
	virtual ~VulkanLayerAndExtension();

	/**
	 * Layers and corresponding extension list
	 */
	vector<LayerProperties> layerProperties;

	/**
	 * List of layer names requested by the application
	 */
	vector<const char*> appRequestedLayerNames;

	/**
	 * List of extension names requested by the application
	 */
	vector<const char*> appRequestedExtensionNames;

	/**
	 *
	 */
	VkResult getInstanceLayerProperties();

	/**
	 * This function retrieves an extension and its properties at instance and device level.
	 * \param layerProps	The LayerProperties to be filled
	 * \param gpu			If set, retrieves device level extensions.
	 */
	VkResult getExtensionProperties(LayerProperties &layerProps, const VkPhysicalDevice*const gpu = nullptr);

	/**
	 * TODO
	 */
	VkResult getDeviceExtensionProperties(VkPhysicalDevice* gpu);
};
