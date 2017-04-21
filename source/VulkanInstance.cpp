/*
 * VulkanInstance.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: marc
 */

#include <VulkanInstance.h>

VulkanInstance::VulkanInstance() {
	// TODO Auto-generated constructor stub

}

VulkanInstance::~VulkanInstance() {
	// TODO Auto-generated destructor stub
}

VkResult VulkanInstance::createInstance(vector<const char*>& layers,
		vector<const char*>& extensions, const char* applicationName) {
	// Set the instance specific layer and extension information
	layerExtension.appRequestedExtensionNames = layers;
	layerExtension.appRequestedLayerNames = layers;

	// Define the Vulkan application structure
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = applicationName;
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = applicationName;
	appInfo.engineVersion = 1;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

	// Define the Vulkan instance create info structure
	VkInstanceCreateInfo instInfo = {};
	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;

	// Specify the list of layers to be enabled
	instInfo.enabledLayerCount = layers.size();
	instInfo.ppEnabledLayerNames = layers.data();

	// Specify the list of extensions to be enabled
	instInfo.enabledExtensionCount = extensions.size();
	instInfo.ppEnabledExtensionNames = extensions.data();

	return vkCreateInstance(&instInfo, nullptr, &instance);
}

void VulkanInstance::destroyInstance() {
	vkDestroyInstance(instance, nullptr);
}
