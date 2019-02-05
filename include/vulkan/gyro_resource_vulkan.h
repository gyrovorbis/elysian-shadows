#ifndef GYRO_RESOURCE_VULKAN_H
#define GYRO_RESOURCE_VULKAN_H

#include <vulkan/gyro_vulkan.h>
#include <vulkan/gyro_util_vulkan.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_VK_RESOURCE_FLAGS_TYPE_BIT           0
#define GY_VK_RESOURCE_FLAGS_INITIALIZED_BIT    1
#define GY_VK_RESOURCE_FLAGS_WRITE_BIT          2
#define GY_VK_RESOURCE_FLAGS_WRITE_CACHE_BIT    3
#define GY_VK_RESOURCE_FLAGS_FORCE_WRITE_BIT    4
#define GY_VK_RESOURCE_FLAGS_DIRTY_BIT          5

typedef uint32_t GYVkResourceFlags;

typedef enum GY_VK_RESOURCE_TYPE {
    GY_VK_RESOURCE_TYPE_BUFFER,
    GY_VK_RESOURCE_TYPE_IMAGE
} GY_VK_RESOURCE_TYPE;

typedef union GYVkResourceHandle {
    VkBuffer    buffer;
    VkImage     image;
} GYVkResourceHandle;

typedef struct GYVkResource {
    GYVkResourceHandle  handle;
    VkDeviceMemory      deviceMemory;
    size_t              size;
    uint8_t*            writeCache;
    GYVkResourceFlags   flags;
    uint32_t            memoryTypeIndex;
} GYVkResource;

typedef struct GYVkImage {
    GYVkResource    resource;
    VkImageLayout   layout;
} GYVkImage;

int             gyVkResourceCreateBuffer(GYVkResource* resource, GYVkResourceFlags resourceFlags, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, void* data);
void            gyVkResourceDestroyBuffer(GYVkResource* resource);
inline void     gyVkResourceFillBuffer(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, uint32_t value);
inline int      gyVkResourceMapMemory(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ptr);
inline void     gyVkResourceUnmapMemory(const GYVkResource* resource);
int             gyVkResourceReadBuffer(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, void* buffer);
int             gyVkResourceWriteBuffer(GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, void* buffer);
inline GY_VK_RESOURCE_TYPE
                gyVkResourceType(const GYVkResource* resource);

void gyVkResourceCopyBufferToImage(const GYVkResource* buff, GYVkImage* image, const VkBufferImageCopy* imageCopy, VkImageLayout imgLayout, VkCommandBuffer copyCmd);








//================ INLINEZ ======================


extern VkInstance                          _vkInstance;
extern VkPhysicalDevice                    _vkPhysicalDevice;
extern VkDevice                            _vkDevice;
extern VkQueue                             _vkQueue;
extern VkCommandBuffer                     _vkCmdBuffer;
extern VkPhysicalDeviceMemoryProperties    _vkPhysicalDevMemProperties;


inline GY_VK_RESOURCE_TYPE gyVkResourceType(const GYVkResource* resource) {
    assert(resource);
    return (GY_VK_RESOURCE_TYPE)(resource->flags & (0x1<<GY_VK_RESOURCE_FLAGS_TYPE_BIT));
}


inline int gyVkResourceMapMemory(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ptr) {
    VkResult result = vkMapMemory(_vkDevice, resource->deviceMemory, offset, size, flags, ptr);
    GY_VK_VERIFY(result);
    return result == VK_SUCCESS;
}

inline void gyVkResourceUnmapMemory(const GYVkResource* resource) {
    vkUnmapMemory(_vkDevice, resource->deviceMemory);
}



inline void gyVkResourceFillBuffer(const GYVkResource* resource, VkDeviceSize offset, VkDeviceSize size, uint32_t value) {
    assert(resource && gyVkResourceType(resource) == GY_VK_RESOURCE_TYPE_BUFFER);
    vkCmdFillBuffer(_vkCmdBuffer,
                    resource->handle.buffer,
                    offset,
                    size,
                    value);
}

#ifdef __cplusplus
}
#endif


#endif // GYRO_RESOURCE_VULKAN_H
