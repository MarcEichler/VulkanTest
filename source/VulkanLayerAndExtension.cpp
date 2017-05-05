/*
 * VulkanLayerAndExtension.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanLayerAndExtension.h>

#include <iostream>

#include <VulkanApplication.h>
#include <cstring>
#include <algorithm>

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

	VkResult result = VK_SUCCESS;

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

VkBool32 VulkanLayerAndExtension::areLayersSupported(vector<const char *> &layerNames) const {
	uint32_t checkCount = (uint32_t) layerNames.size();
	uint32_t layerCount = (uint32_t) layerProperties.size();

	// Check for unsupported layers
	std::vector<const char*> unsupportedLayers;
	for(uint32_t i = 0; i < checkCount; i++) {
		VkBool32 isSupported = 0;
		for (uint32_t j = 0; j < layerCount; j++) {
			if (!strcmp(layerNames[i], layerProperties[j].properties.layerName)) {
				isSupported = 1;
			}
		}
		if (!isSupported) {
			std::cout << "Layer " << layerNames[i] << " is not supported. The layer will be removed." << std::endl;
			unsupportedLayers.push_back(layerNames[i]);
		} else {
			std::cout << "Layer " << layerNames[i] << " is supported." << std::endl;
		}
	}

	// Remove unsupported layers for the list
	for (const auto layer : unsupportedLayers) {
		const auto it = std::find(layerNames.begin(), layerNames.end(), layer);
		if (it != layerNames.end()) layerNames.erase(it);
	}
    std::cout << unsupportedLayers.size() << " layers were unsupported and removed." << std::endl;

	return (VkBool32) true;
}

VkResult VulkanLayerAndExtension::createDebugReportCallback() {
    VkResult result;

    const VulkanApplication& vulkanApplication = *VulkanApplication::GetInstance();
    const VkInstance& vkInstance = vulkanApplication.vulkanInstance.instance;

    // Get vkCreateDebugReportCallbackEXT API
    dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
    if (!dbgCreateDebugReportCallback) {
        std::cout << "Error: vkGetInstanceProcAddr is unable to locate vkCreateDebugReportCallbackEXT!\n";
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "Loaded vkCreateDebugReportCallbackEXT\n";

    // Get vkDestroyDebugReportCallbackEXT API
    dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
    if (!dbgCreateDebugReportCallback) {
        std::cout << "Error: vkGetInstanceProcAddr is unable to locate vkDestroyDebugReportCallbackEXT!\n";
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    std::cout << "Loaded vkDestroyDebugReportCallbackEXT\n";

    // Define the debug report control structure
    dbgReportCreateInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    dbgReportCreateInfo.pfnCallback = &debugFunction;
    dbgReportCreateInfo.pUserData   = nullptr;
    dbgReportCreateInfo.pNext       = nullptr;
    dbgReportCreateInfo.flags       = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                                      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                      VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                      VK_DEBUG_REPORT_DEBUG_BIT_EXT |
                                      VK_DEBUG_REPORT_ERROR_BIT_EXT;

    // Create the debug report callback and store the handle
    result = dbgCreateDebugReportCallback(vkInstance, &dbgReportCreateInfo, nullptr, &debugReportCallback);
    if (result == VK_SUCCESS) {
        std::cout << "Debug report callback object created successfully.\n";
    }

    return result;
}

void VulkanLayerAndExtension::destroyDebugReportCallback() {
    const VulkanApplication& vulkanApplication = *VulkanApplication::GetInstance();
    const VkInstance& vulkanInstance = vulkanApplication.vulkanInstance.instance;
    dbgDestroyDebugReportCallback(vulkanInstance, debugReportCallback, nullptr);
}

VkBool32
VulkanLayerAndExtension::debugFunction(const VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
                                       size_t location, int32_t msgCode, const char *layerPrefix, const char *msg,
                                       void *userData) {
    std::string type;

    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) type = "ERROR";
    else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) type = "WARNING";
    else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) type = "INFORMATION";
    else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) type = "PERFORMANCE";
    else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) type = "DEBUG";
    else
        return VK_FALSE;

    std::cout << "[VK_DEBUG_REPORT] "<<type<<": [" << layerPrefix << "] Code " << msgCode << ":" << msg << std::endl;

    fflush(stdout);
    return VK_TRUE;
}
