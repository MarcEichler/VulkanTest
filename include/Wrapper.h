//
// Created by marc on 5/5/17.
//

#ifndef PENGUIN_WRAPPER_H
#define PENGUIN_WRAPPER_H

#include <vulkan/vulkan.h>

/**
 * \brief Manager for Vulkan command buffers.
 */
class CommandBufferMgr {
public:
    /**
     * \brief Allocate memory for command buffers from the command pool
     * @param device            The Vulkan device to use
     * @param cmdPool           The command pool
     * @param cmdBuf            The command buffer
     * @param commandBufferInfo The command buffer allocate info
     */
    static void allocCommandBuffer(const VkDevice* device,
                                   const VkCommandPool cmdPool,
                                   VkCommandBuffer* cmdBuf,
                                   const VkCommandBufferAllocateInfo* commandBufferInfo);

    /**
     * \brief Start command buffer recording
     * @param cmdBuf                    The command buffer
     * @param commandBufferBeginInfo    The command buffer begin info
     */
    static void beginCommandBuffer(VkCommandBuffer cmdBuf,
                                   const VkCommandBufferBeginInfo*const commandBufferBeginInfo = nullptr);

    /**
     * \brief End command buffer recording
     * @param cmdBuf    The command buffer
     */
    static void endCommandBuffer(VkCommandBuffer cmdBuf);

    /**
     * \brief Submit a command buffer for execution
     * @param queue         The queue to submit to
     * @param cmdBufList    The command buffers to submit
     * @param submitInfo    The submit info
     * @param fence         The fence
     */
    static void submitCommandBuffer(const VkQueue& queue,
                                    const VkCommandBuffer* cmdBufList,
                                    const VkSubmitInfo* submitInfo = nullptr,
                                    const VkFence& fence = VK_NULL_HANDLE);
};
#endif //PENGUIN_WRAPPER_H
