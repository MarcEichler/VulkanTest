/*
 * VulkanLayerAndExtension.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
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
	 * TODO
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VULKAN DEBUGGING MEMBER FUNCTIONS AND VARIABLES
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
	 * \brief Checks whether or not a layer is supported by the vulkan driver implementation.
	 *
	 * Any unsupported layers will be removed from layerNames.
	 *
	 * @param layerNames	The layers
	 * @return	true
	 */
	VkBool32 areLayersSupported(vector<const char*> &layerNames) const;

    /**
     * Create the debug report callback function
     * @return Successful?
     */
    VkResult createDebugReportCallback();

    /**
     * Destroy the debug report callback function
     */
    void destroyDebugReportCallback();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugFunction(const VkFlags msgFlags,
                                                        VkDebugReportObjectTypeEXT objType,
                                                        uint64_t srcObject,
                                                        size_t location,
                                                        int32_t msgCode,
                                                        const char* layerPrefix,
                                                        const char* msg,
                                                        void* userData
    );

private:
    /**
     * Declaration of the debug report callback create function pointer
     */
    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;

    /**
     * Declaration of the debug report callback destroy function pointer
     */
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;

    VkDebugReportCallbackEXT debugReportCallback;

public:
    VkDebugReportCallbackCreateInfoEXT dbgReportCreateInfo = {};
};
