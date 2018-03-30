//
// Created by marc on 5/5/17.
//
#include "Wrappers.h"

#include <assert.h>

void CommandBufferMgr::allocCommandBuffer(const VkDevice *device, const VkCommandPool cmdPool, VkCommandBuffer *cmdBuf,
                                          const VkCommandBufferAllocateInfo* commandBufferInfo) {
    VkResult result;

    // If allocation info is available, use it
    if (commandBufferInfo) {
        result = vkAllocateCommandBuffers(*device, commandBufferInfo, cmdBuf);
    }
    // Otherwise, use default implementation
    else {
        // Create allocate info
        VkCommandBufferAllocateInfo cmdInfo = {};
        cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdInfo.pNext = nullptr;
        cmdInfo.commandPool = cmdPool;
        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdInfo.commandBufferCount = (uint32_t) sizeof(cmdBuf) / sizeof(VkCommandBuffer);

        // Allocate
        result = vkAllocateCommandBuffers(*device, &cmdInfo, cmdBuf);
    }
    assert(!result);

    return;
}

void
CommandBufferMgr::beginCommandBuffer(VkCommandBuffer cmdBuf, const VkCommandBufferBeginInfo*const commandBufferBeginInfo) {
    VkResult result;

    // If a custom command buffer was specified, use it
    if (commandBufferBeginInfo) {
        result = vkBeginCommandBuffer(cmdBuf, commandBufferBeginInfo);
    }
    // Otherwise, use default implementation
    else {
        VkCommandBufferInheritanceInfo inheritanceInfo = {};
        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.pNext = nullptr;
        inheritanceInfo.renderPass = VK_NULL_HANDLE;
        inheritanceInfo.subpass = 0;
        inheritanceInfo.framebuffer = VK_NULL_HANDLE;
        inheritanceInfo.occlusionQueryEnable = VK_FALSE;
        inheritanceInfo.queryFlags = 0;
        inheritanceInfo.pipelineStatistics = 0;

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = &inheritanceInfo;

        result = vkBeginCommandBuffer(cmdBuf, &beginInfo);
    }

    assert(result == VK_SUCCESS);
    return;
}

void CommandBufferMgr::endCommandBuffer(VkCommandBuffer cmdBuf) {
    VkResult result;

    result = vkEndCommandBuffer(cmdBuf);
    assert(result == VK_SUCCESS);

    return;
}

void CommandBufferMgr::submitCommandBuffer(const VkQueue &queue, const VkCommandBuffer *cmdBufList,
                                           const VkSubmitInfo *submitInfo, VkFence const &fence) {
    VkResult result;

    // If submit info is available, use it as it is
    if (submitInfo) {
        result = vkQueueSubmit(queue, 1, submitInfo, fence);
        assert(!result);
    }
    // Otherwise, use default implementation
    else {
        VkSubmitInfo subInfo = {};
        subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        subInfo.pNext = nullptr;
        subInfo.waitSemaphoreCount = 0;
        subInfo.pWaitSemaphores = nullptr;
        subInfo.pWaitDstStageMask = nullptr;
        subInfo.commandBufferCount = (uint32_t) sizeof(cmdBufList) / sizeof(VkCommandBuffer);
        subInfo.pCommandBuffers = cmdBufList;
        subInfo.signalSemaphoreCount = 0;
        subInfo.pSignalSemaphores = nullptr;

        result = vkQueueSubmit(queue, 1, &subInfo, fence);
        assert(!result);

    }

    result = vkQueueWaitIdle(queue);
    assert(!result);

    return;
}
