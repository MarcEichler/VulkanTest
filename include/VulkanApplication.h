/*
 * VulkanApplication.h
 *
 *  Created on: Mar 1, 2017
 *      Author: Marc Eichler
 */

#ifndef INCLUDE_VULKANAPPLICATION_H_
#define INCLUDE_VULKANAPPLICATION_H_

#include <vulkan/vulkan.h>
#include <mutex>
#include <memory>

#include "VulkanInstance.h"
#include "VulkanDevice.h"

using namespace std;

/**
 * \brief A Vulkan rendering application.
 *
 * This class is a singleton.
 */
class VulkanApplication {
private:
	/**
	 * \brief Default constructor
	 */
	VulkanApplication();

public:
	/**
	 * \brief Destructor
	 */
	virtual ~VulkanApplication();

private:
	/**
	 * The singleton object
	 */
	static std::unique_ptr<VulkanApplication> instance;

	/**
	 * Set once the singleton was created
	 */
	static std::once_flag created;
public:
	/**
	 * \brief Returns the singleton object.
	 *
	 * If the singleton was not created, creates it.
	 * @return the singleton
	 */
	static VulkanApplication* GetInstance();

	/**
	 * \brief Initializes the application
	 */
	void initialize();

	/**
	 * \brief TODO
	 */
	void prepare();

	/**
	 * \brief TODO
	 */
	void update();

	/**
	 * \brief TODO
	 * @return
	 */
	bool render();

	/**
	 * \brief De-initializes the application
	 */
	void deInitialize();

private:
	VkResult createVulkanInstance(vector<const char*>& layers, vector<const char*>& extensions,
									const char* applicationName);

	VkResult handShakeWithDevice(VkPhysicalDevice* gpu,
			std::vector<const char *>& layers, std::vector<const char *>& extensions);

	/**
	 * \brief Enumerates the physical devies.
	 *
	 * The devices will be added to gpuList.
	 *
	 * @param gpuList	The list
	 * @return successful?
	 */
	VkResult enumeratePhysicalDevices(vector<VkPhysicalDevice>& gpuList);

public:
	/**
	 * The Vulkan instance
	 */
	VulkanInstance vulkanInstance;

	/**
	 * The Vulkan device
	 */
	VulkanDevice* device;

private:
	/**
	 * True for additional debug info
	 */
	bool debugFlag;
};

#endif /* INCLUDE_VULKANAPPLICATION_H_ */
