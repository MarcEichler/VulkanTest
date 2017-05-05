/*
 * VulkanInstance.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#include <VulkanInstance.h>

VulkanInstance::VulkanInstance() {
	// TODO Auto-generated constructor stub

}

VulkanInstance::~VulkanInstance() {
	// TODO Auto-generated destructor stub
}

VkResult VulkanInstance::createInstance(vector<const char*>& layerNames,
		vector<const char*>& extensions, const char* applicationName) {
	// Set the instance specific layer and extension information
	layerExtension.appRequestedExtensionNames = layerNames;
	layerExtension.appRequestedLayerNames = layerNames;

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
	instInfo.pNext = &layerExtension.dbgReportCreateInfo;
	instInfo.flags = 0;
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
#endif
	instInfo.pApplicationInfo = &appInfo;
#ifdef __clang__
#pragma clang diagnostic pop
#endif

	// Specify the list of layers to be enabled
	instInfo.enabledLayerCount = (uint32_t) layerNames.size();
	instInfo.ppEnabledLayerNames = layerNames.data();

	// Specify the list of extensions to be enabled
	instInfo.enabledExtensionCount = (uint32_t) extensions.size();
	instInfo.ppEnabledExtensionNames = extensions.data();

	return vkCreateInstance(&instInfo, nullptr, &instance);
}

void VulkanInstance::destroyInstance() {
	vkDestroyInstance(instance, nullptr);
}
