#include <vulkan/gyro_resource_vulkan.h>
#include <vulkan/gyro_vulkan.h>
#include <vulkan/gyro_query_vulkan.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static uint32_t         _queueFamilyIndexCount;
static uint32_t*        _queueFamilyIndices;
static VkCommandPool    _commandPool;
static VkQueue          _queue;

extern inline VkResult  gyVkResourceMapMemory(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ptr);
extern inline void      gyVkResourceUnmapMemory(const GYVkResource* resource);
extern inline void      gyVkResourceFillBuffer(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, uint32_t value);
extern inline GY_VK_RESOURCE_TYPE gyVkResourceType(const GYVkResource* resource);

/*
 * Some sort of queue structure where all writes/changes to uniforms and shit get appended to, so that they can all be flushed/written at once for speed?
 *
 */

//TODO
//Image functions
//Descriptor Bindings
//Descriptor Set Layout
//Pipeline Layout
//Binding Descriptor sets to Pipeline
//Shaders in Pipeline
//TEXTURE FUNCTIONS
//Memory coherency/halting on shit/flushing before needing it... memory modification queue? Fences/Barriers for uniform updates?
//Functions for figuring out queueFamilyIndices here and memoryTypeIndex!!!

int gyVkResourceCreateBuffer(GYVkResource* resource, GYVkResourceFlags resourceFlags, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, void* data) {
    VkResult result;

    memset(resource, 0, sizeof(GYVkResource));

    const VkBufferCreateInfo bufferInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   //type
        NULL,                                   //next
        0,                                      //VkBufferCreateFlags
        size,                                   //size
        usageFlags,                             //VkBufferUsageFlagBits
        VK_SHARING_MODE_EXCLUSIVE,              //sharing mode
        _queueFamilyIndexCount,                 //# of queue family indices
        _queueFamilyIndices                     //indices of queue families accessing buffer
    };

    if((result = vkCreateBuffer(_vkDevice, &bufferInfo, NULL, &resource->handle.buffer)) != VK_SUCCESS) {
        goto done;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_vkDevice, resource->handle.buffer, &memRequirements);

    VkBool32 memoryIndexFound;
    const VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        NULL,
        memRequirements.size,
        gyVkMemoryTypeIndexFromTypeProperties(memRequirements.memoryTypeBits, memoryProperties, &memoryIndexFound)
    };

    if(!memoryIndexFound) {
        _gyLog(GY_DEBUG_ERROR, "Vulkan API Error: CreateBuffer failed! Device memory index could not be found meeting the type requirements! [typeBits: %u]", memRequirements.memoryTypeBits);
        GY_VK_ASSERT(0);
    }

    if((result = vkAllocateMemory(_vkDevice, &allocInfo, NULL, &resource->deviceMemory)) != VK_SUCCESS) {
        goto deinit_buffer;
    }

    vkBindBufferMemory(_vkDevice, resource->handle.buffer, resource->deviceMemory, 0);

    if(data) {
        result = gyVkResourceWriteBuffer(resource, 0, size, data);
    }

    if(resourceFlags & (1<<GY_VK_RESOURCE_FLAGS_WRITE_CACHE_BIT)) {
        resource->writeCache = malloc(size);
        if(data) {
            memcpy(resource->writeCache, data, size);
        }
    }

    goto done;

deinit_buffer:
    vkDestroyBuffer(_vkDevice, resource->handle.buffer, NULL);
done:
    GY_VK_VERIFY(result);
    return result == VK_SUCCESS;
}

void gyVkResourceDestroyBuffer(GYVkResource* resource) {
    assert(resource);
    free(resource->writeCache);
    vkFreeMemory(_vkDevice, resource->deviceMemory, NULL);
    vkDestroyBuffer(_vkDevice, resource->handle.buffer, NULL);
}

int gyVkResourceReadBuffer(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, void* buffer) {
    assert(resource);
    VkResult result;
    void* dataPtr;

    if((result = gyVkResourceMapMemory(resource, offset, size, 0, &dataPtr)) == VK_SUCCESS) {
        memcpy(buffer, dataPtr, size);
        gyVkResourceUnmapMemory(resource);
    }

    GY_VK_VERIFY(result);
    return result == VK_SUCCESS;
}

int gyVkResourceWriteBuffer(GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, void* buffer) {
    assert(resource);
    VkResult result = VK_SUCCESS;
    void* dataPtr;

    if(resource->writeCache) {
        if(memcmp(resource->writeCache+offset, buffer, size) == 0)
            return 1;
    }

    if((result = gyVkResourceMapMemory(resource, offset, size, 0, &dataPtr)) == VK_SUCCESS) {
        memcpy(dataPtr, buffer, size);
        gyVkResourceUnmapMemory(resource);

        //have to flush cache manually if memory isn't host-coherent!!!
        if(!_vkPhysicalDevMemProperties.memoryTypes[resource->memoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

            VkMappedMemoryRange mappedMemoryRange = {
                VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                NULL,
                resource->deviceMemory,
                offset,
                size
            };

           result = vkFlushMappedMemoryRanges(_vkDevice, 1, &mappedMemoryRange);
        }
    }
    GY_VK_VERIFY(result);
    return result == VK_SUCCESS;
}

void gyVkResourceCopyBuffer(GYVkResource* dst, GYVkResource* src, VkDeviceSize size) {
    assert(src && dst);

    const VkCommandBufferAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        NULL,
        _commandPool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        1   //cmdBufferCount
    };

    VkCommandBuffer commandBuffer;
    GY_VK_VERIFY(vkAllocateCommandBuffers(_vkDevice, &allocInfo, &commandBuffer));

    const VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        NULL
    };
    GY_VK_VERIFY(vkBeginCommandBuffer(commandBuffer, &beginInfo));

    VkBufferCopy copyRegion = {
        0, //srcOffset
        0, //dstOffset
        size
    };
    vkCmdCopyBuffer(commandBuffer, src->handle.buffer, src->handle.buffer, 1, &copyRegion);

    GY_VK_VERIFY(vkEndCommandBuffer(commandBuffer));

    const VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        NULL,
        0,                  //waitSemaphoreCount
        NULL,               //waitSemaphores
        NULL,               //waitDstStageMask
        1,                  //cmdBufferCount
        &commandBuffer,
        0,                  //signalSemaphoreCount
        NULL                //signalSemaphores
    };
    GY_VK_VERIFY(vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE));
    GY_VK_VERIFY(vkQueueWaitIdle(_queue));

    vkFreeCommandBuffers(_vkDevice, _commandPool, 1, &commandBuffer);
}

void gyVkResourceCopyBufferToImage(const GYVkResource* buff, GYVkImage* image, const VkBufferImageCopy* imageCopy, VkImageLayout imageLayout, VkCommandBuffer copyCmd) {
    assert(buff && image && imageCopy);
    image->layout = imageLayout;
    vkCmdCopyBufferToImage(copyCmd, buff->handle.buffer, image->resource.handle.image, imageLayout, 1, imageCopy);
}







