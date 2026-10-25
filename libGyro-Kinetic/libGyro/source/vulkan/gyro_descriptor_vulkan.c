#include <vulkan/gyro_descriptor_vulkan.h>
#include <vulkan/gyro_resource_vulkan.h>
#include <stdlib.h>
#include <string.h>

const static VkDescriptorPoolSize _descriptorPoolSizes[] = {
    {
        VK_DESCRIPTOR_TYPE_SAMPLER,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        0
    },
    {
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
        10
    },
    {   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        10
    },
    {
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        0
    },
    {
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
        0
    },
    {
        VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
        10
    }
};

static const uint32_t   _maxPoolSets = 10;
static VkDescriptorPool _descriptorPool;



//============ DescriptorSetLayout ============

//Inline instantiations

inline void gyVkDescriptorSetLayoutDestroy(GYVkDescriptorSetLayout* layout) {
    assert(layout);
    free(layout->descriptorBindings);
    vkDestroyDescriptorSetLayout(_vkDevice, layout->descriptorSetLayout, NULL);
}

inline extern uint32_t gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(const GYVkDescriptorSetLayout* layout, uint32_t descriptorBinding);

int gyVkDescriptorSetLayoutCreate(GYVkDescriptorSetLayout* layout, uint32_t binding, uint32_t descriptorBindingCount, VkDescriptorSetLayoutBinding* descriptorBindings) {
    assert(layout && descriptorBindingCount && descriptorBindings);

    layout->binding = binding;
    layout->descriptorBindingCount = descriptorBindingCount;

    //copy them internally, so application doesn't have to manage their memory
    layout->descriptorBindings = malloc(sizeof(VkDescriptorSetLayoutBinding) * descriptorBindingCount);
    memcpy(layout->descriptorBindings, descriptorBindings, sizeof(VkDescriptorSetLayoutBinding) * descriptorBindingCount);

    const VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        NULL,
        0,  //flags, not used by API yet
        descriptorBindingCount,
        descriptorBindings
    };
    return GY_VK_VERIFY(vkCreateDescriptorSetLayout(_vkDevice, &setLayoutCreateInfo, NULL, &layout->descriptorSetLayout));
}

//============ DescriptorSetPool ============

//Inline instantiation
inline extern void  gyVkDescriptorPoolDestroy(void);

int gyVkDescriptorPoolCreate(void) {
    const VkDescriptorPoolCreateInfo poolInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        NULL,
        0, //Application CANNOT free single descriptors at a time. Operate on whole set!
        _maxPoolSets,
        sizeof(_descriptorPoolSizes)/sizeof(VkDescriptorPoolSize),
        _descriptorPoolSizes
    };
    return GY_VK_VERIFY(vkCreateDescriptorPool(_vkDevice, &poolInfo, NULL, &_descriptorPool));
}

void gyVkDescriptorPoolDestroy(void) {
    vkDestroyDescriptorPool(_vkDevice, _descriptorPool, NULL);
}

//============ DescriptorSet ============

int gyVkDescriptorSetsCreate(GYVkDescriptorSet** sets, uint32_t setCount, const GYVkDescriptorSetLayout** layouts, const GYVkDescriptorSetFlags* flags) {
    assert(sets && setCount && layouts);

    VkDescriptorSet         descriptorSets[setCount];
    VkDescriptorSetLayout   descriptorSetLayouts[setCount];

    //Initialize GYVkDescriptorSetFlags and internal data
    for(unsigned i = 0; i < setCount; ++i) {
        GYVkDescriptorSet*const curSet = sets[i];
        memset(curSet, 0, sizeof(GYVkDescriptorSet));

        //set layout for the set
        curSet->layout = layouts[i];
        curSet->flags  = 0;

        //Initialize flags
        if(flags && flags[i]) {
            curSet->flags = flags[i];

            //Initialize write cache (if enabled)
            if(flags[i] & (1<<GY_VK_DESCRIPTOR_SET_FLAGS_WRITE_CACHE_BIT)) {
                curSet->writeCaches = malloc(sizeof(GYVkDescriptorWriteCacheData*) * layouts[i]->descriptorBindingCount);

                for(unsigned j = 0; j < layouts[i]->descriptorBindingCount; ++j) {
                    curSet->writeCaches[j] = malloc(sizeof(GYVkDescriptorWriteCacheData));
                    curSet->writeCaches[j]->infoType = GY_VK_DESCRIPTOR_INFO_TYPE_NULL;
                }
            }
        }

        //Gather layouts into one contiguous array for Vulkan while we're already iterating...
        descriptorSetLayouts[i] = layouts[i]->descriptorSetLayout;
    }

    const VkDescriptorSetAllocateInfo descSetAllocInfo = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        NULL,
        _descriptorPool,
        setCount,
        descriptorSetLayouts
    };
    int success = GY_VK_VERIFY(vkAllocateDescriptorSets(_vkDevice, &descSetAllocInfo, descriptorSets));

    //Scatter layouts back into objects from contiguous Vulkan output array
    for(unsigned i = 0; i < setCount; ++i) {
        sets[i]->descriptorSet = descriptorSets[i];
        sets[i]->layout = layouts[i];
    }

    return success;
}

int gyVkDescriptorSetsDestroy(uint32_t setCount, GYVkDescriptorSet* sets) {
    assert(setCount && sets);

    for(unsigned i = 0; i < setCount; ++i) {
        for(unsigned j = 0; j < sets[i].layout->descriptorBindingCount; ++j)
            free(sets[i].writeCaches[j]);

        free(sets[i].writeCaches);
    }

    //Gather sets into single array for Vulkan argument
    VkDescriptorSet descriptorSets[setCount];
    for(unsigned i = 0; i < setCount; ++i) descriptorSets[i] = sets->descriptorSet;

    VkResult result = vkFreeDescriptorSets(_vkDevice, _descriptorPool, setCount, descriptorSets);
    GY_VK_VERIFY(result);
    return result == VK_SUCCESS;
}



void gyVkDescriptorSetWriteToVkWriteDescriptorSet(VkWriteDescriptorSet* dstWriteSet, const GYVkDescriptorSetWrite* write) {
    VkDescriptorImageInfo imgInfo;
    VkDescriptorBufferInfo buffInfo;

    const GYVkDescriptorImageInfo*const gyImgInfo = write->imageInfo;
    const GYVkDescriptorBufferInfo*const gyBuffInfo = write->bufferInfo;

    if(gyImgInfo) {
        VkDescriptorImageInfo tmpImgInfo = {
            gyImgInfo->sampler,
            gyImgInfo->imageView,
            gyImgInfo->imageLayout
        };
        imgInfo = tmpImgInfo;

    } else if(gyBuffInfo) {
        VkDescriptorBufferInfo tmpBuffInfo = {
            gyBuffInfo->resource->handle.buffer,
            gyBuffInfo->offset,
            gyBuffInfo->range
        };
        buffInfo = tmpBuffInfo;
    }

    const GYVkDescriptorSetLayout* layout = write->descriptorSet->layout;

    VkWriteDescriptorSet tmpSet = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        NULL,
        write->descriptorSet->descriptorSet,
        write->dstBinding,
        write->dstArrayElement,
        write->descriptorCount,
        layout->descriptorBindings[gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(layout, write->dstBinding)].descriptorType,
        gyImgInfo? &imgInfo : NULL,
        gyBuffInfo? &buffInfo : NULL,
        write->texelBufferView
    };
    *dstWriteSet = tmpSet;
}

void gyVkDescriptorSetCopyToVkCopyDescriptorSet(VkCopyDescriptorSet* dstCopySet, const GYVkDescriptorSetCopy* copy) {
    VkCopyDescriptorSet tmp = {
        VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
        NULL,
        copy->srcSet->descriptorSet,
        copy->srcBinding,
        copy->srcArrayElement,
        copy->dstSet->descriptorSet,
        copy->dstBinding,
        copy->dstArrayElement,
        copy->descriptorCount
    };
    *dstCopySet = tmp;
}


void gyVkDescriptorSetWriteCacheForceWrite(GYVkDescriptorWriteCacheData* cacheData, const GYVkDescriptorSetWrite* write, const GYVkDescriptorSetCopy* copy) {
    assert(cacheData && (write || copy) && !(write && copy)); //can't write AND copy at the same time OR do neither, fuckweasel

    memset(cacheData, 0, sizeof(GYVkDescriptorWriteCacheData));

    if(write) {
        cacheData->dstArrayElement = write->dstArrayElement;
        cacheData->descriptorCount = write->descriptorCount;

        if(write->bufferInfo) {
            cacheData->infoType = GY_VK_DESCRIPTOR_INFO_TYPE_BUFFER;
            memcpy(&cacheData->info.buffer, write->bufferInfo, sizeof(GYVkDescriptorBufferInfo));
        } else if(write->imageInfo) {
            cacheData->infoType = GY_VK_DESCRIPTOR_INFO_TYPE_IMAGE;
            memcpy(&cacheData->info.image, write->imageInfo, sizeof(GYVkDescriptorBufferInfo));
        } else if(write->texelBufferView) {
            cacheData->infoType = GY_VK_DESCRIPTOR_INFO_TYPE_VIEW;
            cacheData->info.texelBufferView = write->texelBufferView;
        }

    } else {
        cacheData->infoType                     = GY_VK_DESCRIPTOR_INFO_TYPE_COPY;
        cacheData->dstArrayElement              = copy->dstArrayElement;
        cacheData->descriptorCount              = copy->descriptorCount;
        cacheData->info.copy.srcArrayElement    = copy->srcArrayElement;
        cacheData->info.copy.srcBinding         = copy->srcBinding;
        cacheData->info.copy.srcSet             = copy->srcSet;
    }

}

int gyVkDescriptorSetWriteCacheDataUpdateCompare(GYVkDescriptorWriteCacheData* cacheData, const GYVkDescriptorSetWrite* write, const GYVkDescriptorSetCopy* copy) {
    assert((write || copy) && !(write && copy)); //can't write AND copy at the same time OR do neither, fuckweasel
    if(!cacheData) return 1;

    GYVkDescriptorWriteCacheData newCacheData;
    gyVkDescriptorSetWriteCacheForceWrite(&newCacheData, write, copy);

    //Compare what the new cache entry would be to what it currently is
    if(memcmp(cacheData, &newCacheData, sizeof(GYVkDescriptorWriteCacheData)) != 0) {
        memcpy(cacheData, &newCacheData, sizeof(GYVkDescriptorWriteCacheData));
        return 1;
    }
    return 0;

}

//Returns the number of updates actually commited to memory, once caching and everything is taken into consideration (only data that has changed)
int gyVkDescriptorSetsUpdate(uint32_t writeCount, const GYVkDescriptorSetWrite* writes, uint32_t copyCount, const GYVkDescriptorSetCopy* copies) {
    assert((writeCount && writes) || (copyCount && copies));

    uint32_t dirtyWriteCount = 0;
    uint32_t dirtyCopyCount = 0;
    VkWriteDescriptorSet writeDescriptorSets[writeCount];
    VkCopyDescriptorSet  copyDescriptorSets[copyCount];

    for(unsigned i = 0; i < writeCount; ++i) {
        const GYVkDescriptorSet* const set = writes[i].descriptorSet;
        const uint32_t bindingIndex = gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(set->layout, writes[i].dstBinding);
        if(gyVkDescriptorSetWriteCacheDataUpdateCompare(set->writeCaches[bindingIndex], &writes[i], NULL)) {
            gyVkDescriptorSetWriteToVkWriteDescriptorSet(&writeDescriptorSets[dirtyWriteCount++], &writes[i]);
        }
    }

    for(unsigned i = 0; i < copyCount; ++i) {
        const GYVkDescriptorSet* const set = copies[i].dstSet;
        const uint32_t bindingIndex = gyVkDescriptorSetLayoutFindDescriptorBindingIndexByBinding(set->layout, copies[i].dstBinding);
        if(gyVkDescriptorSetWriteCacheDataUpdateCompare(set->writeCaches[bindingIndex], NULL, &copies[i])) {
            gyVkDescriptorSetCopyToVkCopyDescriptorSet(&copyDescriptorSets[dirtyCopyCount++], &copies[i]);
        }
    }

    vkUpdateDescriptorSets(_vkDevice, dirtyWriteCount, writeDescriptorSets, dirtyCopyCount, copyDescriptorSets);
    return dirtyWriteCount + dirtyCopyCount;
}






