/*
 * VulkanLayerAndExtension.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanLayerAndExtension.h>

#include <iostream>

#include <VulkanApplication.h>

VulkanLayerAndExtension::VulkanLayerAndExtension() {
	// TODO Auto-generated constructor stub

}

VulkanLayerAndExtension::~VulkanLayerAndExtension() {
	// TODO Auto-generated destructor stub
}

VkResult VulkanLayerAndExtension::getInstanceLayerProperties() {
	// Stores number of instance layers
	uint32_t instanceLayerCount;

	// Vector to store layer properties
	vector<VkLayerProperties> layerProperties;

	// Check Vulkan API result status
	VkResult result;

	// Query all layers
	do {
		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

		if (result) return result;

		if (instanceLayerCount == 0)
			return VK_INCOMPLETE;

		layerProperties.resize(instanceLayerCount);

		result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
	} while (result == VK_INCOMPLETE);

	cout << "*** Instanced Layers" << endl;
	for (auto globalLayerProp : layerProperties) {
		cout << "\t" << globalLayerProp.description << endl;
		cout << "\t\t|--[Layer Name]--> " << globalLayerProp.layerName << endl;

		LayerProperties layerProps;
		layerProps.properties = globalLayerProp;

		// Get instance level extensions for corresponding layer properties
		result = this->getExtensionProperties(layerProps);

		if (result) continue;

		this->layerProperties.push_back(layerProps);

		// Print extension name for each instance layer
		for (auto j : layerProps.extensions) {
			cout << "\t\t\t|--[Layer Extensions]--> " << j.extensionName << "\n";
		}
	}
	cout << endl;

	return result;
}

VkResult VulkanLayerAndExtension::getExtensionProperties(
		LayerProperties& layerProps, const VkPhysicalDevice*const gpu) {
	// Stores the number of extension per layer
	uint32_t extensionCount;
	VkResult result;

	// Layer name
	const char* layerName = layerProps.properties.layerName;

	do {
		if (gpu) {
			result = vkEnumerateDeviceExtensionProperties(*gpu, layerName, &extensionCount, nullptr);
		} else {
			result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr);
		}

		if (result || extensionCount == 0) continue;

		layerProps.extensions.resize(extensionCount);

		// Gather all extension properties
		if (gpu) {
			result = vkEnumerateDeviceExtensionProperties(*gpu, layerName,
						&extensionCount, layerProps.extensions.data());
		} else {
			result = vkEnumerateInstanceExtensionProperties(layerName,
						&extensionCount, layerProps.extensions.data());
		}

	} while (result == VK_INCOMPLETE);

	return result;
}

VkResult VulkanLayerAndExtension::getDeviceExtensionProperties(
		VkPhysicalDevice* gpu) {

	VkResult result;

	cout << "***Device Extensions" << endl;
	VulkanApplication* app = VulkanApplication::GetInstance();
	vector<LayerProperties>* instanceLayerProperties = &app->vulkanInstance.layerExtension.layerProperties;

	for (auto globalLayerProp : *instanceLayerProperties) {
		LayerProperties layerProps;
		layerProps.properties = globalLayerProp.properties;

		result = getExtensionProperties(layerProps, gpu);
		if (result) continue;

        std::cout << "\t" << globalLayerProp.properties.description << " ("
        		<< globalLayerProp.properties.layerName << ")" << endl;
        this->layerProperties.push_back(layerProps);

        if (layerProps.extensions.size()) {
                for (auto j : layerProps.extensions) {
                        std::cout << "\t|---[Device Extension]--> " << j.extensionName << endl;
                }
        } else std::cout << "\t|---[Device Extension]--> No extension found" << endl;
	}
	cout << endl;

	return result;
}
