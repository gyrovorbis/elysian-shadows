/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_DESCRIPTOR_VULKAN_H
#define GYRO_DESCRIPTOR_VULKAN_H

#include <vulkan/gyro_vulkan.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_VK_DESCRIPTOR_SET_FLAGS_INITIALIZED_BIT    0
#define GY_VK_DESCRIPTOR_SET_FLAGS_WRITE_BIT          1
#define GY_VK_DESCRIPTOR_SET_FLAGS_WRITE_CACHE_BIT    2
#define GY_VK_DESCRIPTOR_SET_FLAGS_FORCE_WRITE_BIT    3
#define GY_VK_DESCRIPTOR_SET_FLAGS_DIRTY_BIT          4

struct GYVkResource;

typedef struct GYVkDescriptorSetLayout {
    VkDescriptorSetLayout           descriptorSetLayout;
    uint32_t                        binding;
    uint32_t                        descriptorBindingCount;
    VkDescriptorSetLayoutBinding*   descriptorBindings;
} GYVkDescriptorSetLayout;

typedef struct GYVkDescriptorBufferInfo {
    struct GYVkResource*    resource;
    VkDeviceSize            offset;
    VkDeviceSize            range;
} GYVkDescriptorBufferInfo;

typedef struct GYVkDescriptorImageInfo {
    VkSampler       sampler;
    VkImageView     imageView;
    VkImageLayout   imageLayout;
} GYVkDescriptorImageInfo;

struct GYVkDescriptorSet;

typedef struct GYVkDescriptorCopyInfo {
    struct GYVkDescriptorSet*   srcSet;
    uint32_t                    srcBinding;
    uint32_t                    srcArrayElement;    //start element for arrays (of dest)
} GYVkDescriptorCopyInfo;

typedef union GYVkDescriptorInfo {
    GYVkDescriptorImageInfo     image;
    GYVkDescriptorBufferInfo    buffer;
    GYVkDescriptorCopyInfo      copy;
    VkBufferView*               texelBufferView;
} GYVkDescriptorInfo;

typedef enum GY_VK_DESCRIPTOR_INFO_TYPE {
    GY_VK_DESCRIPTOR_INFO_TYPE_NULL,
    GY_VK_DESCRIPTOR_INFO_TYPE_BUFFER,
    GY_VK_DESCRIPTOR_INFO_TYPE_IMAGE,
    GY_VK_DESCRIPTOR_INFO_TYPE_VIEW,
    GY_VK_DESCRIPTOR_INFO_TYPE_COPY
} GY_VK_DESCRIPTOR_INFO_TYPE;

typedef struct GYVkDescriptorWriteCacheData {
    GYVkDescriptorInfo          info;
    GY_VK_DESCRIPTOR_INFO_TYPE  infoType;
    uint32_t                    dstArrayElement;
    uint32_t                    descriptorCount;
} GYVkDescriptorWriteCacheData;

typedef uint32_t GYVkDescriptorSetFlags;

typedef struct GYVkDescriptorSet {
    VkDescriptorSet                 descriptorSet;
    const GYVkDescriptorSetLayout*  layout;
    GYVkDescriptorSetFlags          flags;
    GYVkDescriptorWriteCacheData**  writeCaches;
    //uint32_t            descriptorCount;
    //GYVkDescriptor*     descriptors;
} GYVkDescriptorSet;


typedef struct GYVkDescriptorSetWrite {
    GYVkDescriptorSet*          descriptorSet;
    uint32_t                    dstBinding;
    uint32_t                    dstArrayElement;   //start element for arrays
    uint32_t                    descriptorCount;   //number of elements for arrays
    GYVkDescriptorImageInfo*    imageInfo;
    GYVkDescriptorBufferInfo*   bufferInfo;
    VkBufferView*               texelBufferView;
} GYVkDescriptorSetWrite;

typedef struct GYVkDescriptorSetCopy {
    GYVkDescriptorSet*          srcSet;
    uint32_t                    srcBinding;
    uint32_t                    srcArrayElement;    //start element for arrays (of source)
    GYVkDescriptorSet*          dstSet;
    uint32_t                    dstBinding;
    uint32_t                    dstArrayElement;    //start element for arrays (of dest)
    uint32_t                    descriptorCount;    //number of elements for arrays
} GYVkDescriptorSetCopy;


//====== DescriptorSetLayout ======
int             gyVkDescriptorSetLayoutCreate(GYVkDescriptorSetLayout* layout, uint32_t binding, uint32_t descriptorBindingCount, VkDescriptorSetLayoutBinding* descriptorBindings);
void            gyVkDescriptorSetLayoutDestroy(GYVkDescriptorSetLayout* layout);
inline uint32_t gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(const GYVkDescriptorSetLayout* layout, uint32_t descriptorBinding);


//====== DescriptorPool ======
int     gyVkDescriptorPoolCreate(void);
void    gyVkDescriptorPoolDestroy(void);

//====== DescriptorSet ======
int     gyVkDescriptorSetsCreate(GYVkDescriptorSet** sets, uint32_t setCount, const GYVkDescriptorSetLayout** layouts, const GYVkDescriptorSetFlags* flags);
int     gyVkDescriptorSetsDestroy(uint32_t setCount, GYVkDescriptorSet* sets);
int     gyVkDescriptorSetsUpdate(uint32_t writeCount, const GYVkDescriptorSetWrite* writes, uint32_t copyCount, const GYVkDescriptorSetCopy* copies);

void    gyVkDescriptorSetWriteToVkWriteDescriptorSet(VkWriteDescriptorSet* dstWriteSet, const GYVkDescriptorSetWrite* write);
void    gyVkDescriptorSetCopyToVkCopyDescriptorSet(VkCopyDescriptorSet* dstCopySet, const GYVkDescriptorSetCopy* copy);
void    gyVkDescriptorSetWriteCacheForceWrite(GYVkDescriptorWriteCacheData* cacheData, const GYVkDescriptorSetWrite* write, const GYVkDescriptorSetCopy* copy);
int     gyVkDescriptorSetWriteCacheDataUpdateCompare(GYVkDescriptorWriteCacheData* cacheData, const GYVkDescriptorSetWrite* write, const GYVkDescriptorSetCopy* copy);






// ====== INLINE IMPLEMENTATIONS ======
inline uint32_t gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(const GYVkDescriptorSetLayout* layout, uint32_t descriptorBinding) {
    const uint32_t bindingCount = layout->descriptorBindingCount;
    for(unsigned i = 0; i < bindingCount; ++i) {
        const VkDescriptorSetLayoutBinding* curBinding = &layout->descriptorBindings[i];
        if(curBinding->binding == descriptorBinding) {
            return i;
        }
    }
    assert(0); //never found the binding information for this bitch!
    return 0;
}


#ifdef __cplusplus
}
#endif


#endif // GYRO_DESCRIPTOR_VULKAN_H
