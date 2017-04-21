/*
 * VulkanApplication.h
 *
 *  Created on: Mar 1, 2017
 *      Author: marc
 */

#ifndef INCLUDE_VULKANAPPLICATION_H_
#define INCLUDE_VULKANAPPLICATION_H_

#include <mutex>
#include <memory>

#include "VulkanInstance.h"
#include "VulkanDevice.h"

using namespace std;

class VulkanApplication {
private:
	/**
	 * Default constructor
	 */
	VulkanApplication();

public:
	/**
	 * Destructor
	 */
	virtual ~VulkanApplication();

private:
	static std::unique_ptr<VulkanApplication> instance;
	static std::once_flag created;
public:
	static VulkanApplication* GetInstance();

	void initialize();
	void prepare();
	void update();
	bool render();
	void deInitialize();

	VkResult createVulkanInstance(vector<const char*>& layers, vector<const char*>& extensions,
									const char* applicationName);

	VkResult handShakeWithDevice(VkPhysicalDevice* gpu,
			std::vector<const char *>& layers, std::vector<const char *>& extensions);

	VkResult enumeratePhysicalDevices(vector<VkPhysicalDevice>& gpuList);

	VulkanInstance vulkanInstance;

	VulkanDevice* device;
};

#endif /* INCLUDE_VULKANAPPLICATION_H_ */
