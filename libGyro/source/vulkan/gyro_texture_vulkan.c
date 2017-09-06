#include <vulkan/gyro_video_vulkan.h>
#include <vulkan/gyro_resource_vulkan.h>
#include <vulkan/gyro_device_vulkan.h>
#include <vulkan/gyro_context_vulkan.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>\

typedef struct VulkanTexture {
VkImage         image;
VkImageLayout   layout;
VkDeviceMemory  deviceMemory;
VkImageView     view;
uint32_t        mipLevels;
} VulkanTexture;

#define GY_VK_TEXTURE_FORMAT_DEFAULT            VK_FORMAT_R8G8B8A8_UINT
#define GY_VK_TEXTURE_QUEUE_FAMILY_INDEX_COUNT  1
static uint32_t _queueFamilyIndices[] = { 0 };
static VkCommandPool _commandPool;
static VkQueue          _commandQueue;


VkCommandBuffer gyVkCommandBufferCreate(VkCommandBufferLevel level, int begin) {
    VkCommandBuffer cmdBuffer;

    const VkCommandBufferAllocateInfo cmdBuffAllocateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        NULL,
        _commandPool,
        level,
        1               //count
    };

    GY_VK_VERIFY(vkAllocateCommandBuffers(_vkDevice, &cmdBuffAllocateInfo, &cmdBuffer));

    if(begin) {
        const VkCommandBufferBeginInfo beginInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            NULL,
            0,      //VkCommandBufferUsageFlags
            NULL    //VkCommandBufferInheritanceInfo*
        };

        GY_VK_VERIFY(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
    }

    return cmdBuffer;
}

void gyVkCommandBufferFlush(VkCommandBuffer commandBuffer, VkQueue queue, int free) {
    if(!commandBuffer) {
        return;
    }

    GY_VK_VERIFY(vkEndCommandBuffer(commandBuffer));

    const VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        NULL,
        0,
        NULL,
        NULL,
        1,
        &commandBuffer,
        0,
        NULL
    };

    GY_VK_VERIFY(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    GY_VK_VERIFY(vkQueueWaitIdle(queue));

    if(free) {
        vkFreeCommandBuffers(_vkDevice, _commandPool, 1, &commandBuffer);
    }
}



void gyVkImageLayoutSet(GYVkImage* image, VkImageLayout layout, const VkImageSubresourceRange* subresourceRange, VkCommandBuffer cmdBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
    assert(image && gyVkResourceType(&image->resource) == GY_VK_RESOURCE_TYPE_IMAGE);

    if(image->layout == layout) return;

    VkAccessFlags srcAccessMask;
    VkAccessFlags dstAccessMask;

    // Source layouts (old)
    switch (image->layout) {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        // Only valid as initial layout, memory contents are not preserved
        // Can be accessed directly, no source dependency required
        srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes to the image have been finished
        srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Old layout is transfer destination
        // Make sure any writes to the image have been finished
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    default: assert(0); //UNHANDLED SHIT!!!
    }

    // Target layouts (new)
    switch (layout) {
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Transfer source (copy, blit)
        // Make sure any reads from the image have been finished
        dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Transfer destination (copy, blit)
        // Make sure any writes to the image have been finished
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Shader read (sampler, input attachment)
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    default: assert(0); //UNHANDLED SHIT!!!!
    }

    // Create an image barrier object
    const VkImageMemoryBarrier imageMemoryBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        NULL,
        srcAccessMask,                  //srcAccessMask
        dstAccessMask,                  //dstAccessMask
        image->layout,                  //oldLayout
        layout,                         //newLayout
        0,                              //srcQueueFamilyIndex
        0,                              //dstQueueFamilyIndex
        image->resource.handle.image,
        *subresourceRange
    };

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, //srcStageFlags
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, //dstStageFlags
        0,
        0, NULL,
        0, NULL,
        1, &imageMemoryBarrier);
}


/*
 *  TODO
 * Raw data texture object
 * Desired main format
 * heapMemoryIndex from format
 * createCommandBuffer
 * setLayoutI did the
 * Flush command buffer
 * VulkanDevice->features for anisotropic filtering!
 */
#define SHIT_NOT_FIXED
void _gyVidVulkanTextAlloc(GYTexture *const tex, VkMemoryPropertyFlags memoryProperties) {
    assert(tex);

    void* imgData = NULL;

    GYVkImage* gyImage;

    VulkanTexture* vulkanTex = malloc(sizeof(VulkanTexture));
    tex->_data = vulkanTex;
    //texture2d tex2D(tex->filename); LOAD FUCKING TEXTURE!!!!

    VkFormatProperties formatProperties;

    // calculate num of mip maps
    // numLevels = 1 + floor(log2(max(w, h, d)))
    // Calculated as log2(max(width, height, depth))c + 1 (see specs)
    vulkanTex->mipLevels = floor(log2((tex->width > tex->height)? tex->width : tex->height)) + 1;

    // Get device properites for the requested texture format
    vkGetPhysicalDeviceFormatProperties(_vkPhysicalDevice, GY_VK_TEXTURE_FORMAT_DEFAULT, &formatProperties);

    // Mip-chain generation requires support for blit source and destination
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

    GYVkResource stagingBuffer;
    if(!gyVkResourceCreateBuffer(&stagingBuffer, 0, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tex->bytes, imgData)) {
        tex->_data = NULL;
        return;
    }

    // Create optimal tiled target image
    const VkImageCreateInfo imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        NULL,
        0, //VkImageCreateFlags
        VK_IMAGE_TYPE_2D,
        GY_VK_TEXTURE_FORMAT_DEFAULT,
        {
            tex->width,
            tex->height,
            1
        },
        vulkanTex->mipLevels,
        1, //array layers
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        GY_VK_TEXTURE_QUEUE_FAMILY_INDEX_COUNT,
        _queueFamilyIndices,
        VK_IMAGE_LAYOUT_UNDEFINED
    };

    if(!GY_VK_VERIFY(vkCreateImage(_vkDevice, &imageCreateInfo, NULL, &vulkanTex->image))) {
        goto destroy_buffer;
    }

    VkMemoryRequirements memReqs;
    VkBool32 memoryTypeIndexFound;
    vkGetImageMemoryRequirements(_vkDevice, vulkanTex->image, &memReqs);
    const VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        NULL,
        memReqs.size,
        gyVkPhysicalDeviceMemoryTypeIndexFromTypeProperties(_gyVkPhysicalDevice, memReqs.memoryTypeBits, memoryProperties, &memoryTypeIndexFound)
    };

    if(!memoryTypeIndexFound || !GY_VK_VERIFY(vkAllocateMemory(_vkDevice, &memAllocInfo, NULL, &vulkanTex->deviceMemory))) {
        goto destroy_image;
    }

    if(GY_VK_VERIFY(vkBindImageMemory(_vkDevice, vulkanTex->image, vulkanTex->deviceMemory, 0))) {
        goto destroy_device_mem;

    }

    VkCommandBuffer copyCmd = gyVkCommandBufferCreate(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    const VkImageSubresourceRange subresourceRange = {
        VK_IMAGE_ASPECT_COLOR_BIT,  //aspectMask
        0,                          //baseMipLevel
        1,                          //levelCount
        0,                          //baseArrayLayer
        1                           //layerCount
    };

    gyVkImageLayoutSet(gyImage, VK_IMAGE_LAYOUT_UNDEFINED, &subresourceRange, copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    // Copy the first mip of the chain, remaining mips will be generated
    const VkBufferImageCopy bufferCopyRegion = {
        0,                              //bufferOffset
        0,                              //bufferRowLength
        0,                              //bufferImageHeight,
        {                               //subresourceLayers
            VK_IMAGE_ASPECT_COLOR_BIT,  //aspectMask
            0,                          //mipLevel
            0,                          //baseArrayLayer
            1                           //layerCount
        },
        {                               //imageOffset (3D)
            0, 0, 0
        },
        {                               //imageExtent
            tex->width, tex->height, 1
        }
    };

    gyVkResourceCopyBufferToImage(stagingBuffer.handle.buffer, gyImage, &bufferCopyRegion, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyCmd);

    gyVkImageLayoutSet(gyImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &subresourceRange, copyCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    gyVkCommandBufferFlush(copyCmd, _commandQueue, 1);
    gyVkResourceDestroyBuffer(&stagingBuffer);

    // Generate the mip chain
    // ---------------------------------------------------------------
    // We copy down the whole mip chain doing a blit from mip-1 to mip
    // An alternative way would be to always blit from the first mip level and sample that one down
    VkCommandBuffer blitCmd = gyVkCommandBufferCreate(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    // Copy down mips from n-1 to n
    for (uint32_t i = 1; i < vulkanTex->mipLevels; i++) {
        const VkImageBlit imageBlit = {
            {
                VK_IMAGE_ASPECT_COLOR_BIT,  //aspect
                i-1,                        //mipLevel
                0,                          //baseArrayCount
                1
            },
            {                               //VkOffset3d srcOffsets[2]
                { 0, 0, 0 },                      //srcOffsets[2]
                { (int32_t)(tex->width >> (i - 1)), (int32_t)(tex->height >> (i - 1)), 1 }
            },
            {
                VK_IMAGE_ASPECT_COLOR_BIT,
                i,
                0,
                1
            },
            {
                { 0, 0, 0 },
                { (int32_t)(tex->width >> i), (int32_t)(tex->height >> i), 1 }
            }
        };
        VkImageSubresourceRange mipSubRange;
        memset(&mipSubRange, 0, sizeof(VkImageSubresourceRange));
        mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        mipSubRange.baseMipLevel = i;
        mipSubRange.levelCount = 1;
        mipSubRange.layerCount = 1;

        gyVkImageLayoutSet(gyImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &mipSubRange, blitCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

#ifndef SHIT_NOT_FIXED

        // Blit from previous level
        vkCmdBlitImage(
            blitCmd,
            vulkanTex->image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            vulkanTex->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlit,
            VK_FILTER_LINEAR);

        // Transiton current mip level to transfer source for read in next iteration
        vks::tools::setImageLayout(
            blitCmd,
            vulkanTex->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            mipSubRange,
            VK_PIPELINE_STAGE_HOST_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT);
        #endif
    }

#ifndef SHIT_NOT_FIXED
    // After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
    subresourceRange.levelCount = vulkanTex->mipLevels;
    vks::tools::setImageLayout(
        blitCmd,
        vulkanTex->image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        vulkanTex->layout,
        subresourceRange);

    //FIXME!!!
    VulkanExampleBase::flushCommandBuffer(blitCmd, _vkQueue, true);
    // ---------------------------------------------------------------
#endif

    VkSampler samplers[3];
    memset(&samplers, 0, sizeof(VkSampler)*3);

    // Create samplers
    VkSamplerCreateInfo sampler;
    memset(&sampler, 0, sizeof(VkSampler));
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.magFilter = VK_FILTER_LINEAR;
    sampler.minFilter = VK_FILTER_LINEAR;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler.mipLodBias = 0.0f;
    sampler.compareOp = VK_COMPARE_OP_NEVER;
    sampler.minLod = 0.0f;
    sampler.maxLod = 0.0f;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    sampler.maxAnisotropy = 1.0;
    sampler.anisotropyEnable = VK_FALSE;

    // Without mip mapping
    if(vkCreateSampler(_vkDevice, &sampler, NULL, &samplers[0]) != VK_SUCCESS) _gyLog(GY_DEBUG_ERROR, "REESTED!!!");

    // With mip mapping
    sampler.maxLod = (float)vulkanTex->mipLevels;
    if(vkCreateSampler(_vkDevice, &sampler, NULL, &samplers[1]) != VK_SUCCESS) _gyLog(GY_DEBUG_ERROR, "REESTED!!!");

#ifndef SHIT_NOT_FIXED
    // With mip mapping and anisotropic filtering DEREEST ME!!!!!
    if (vulkanDevice->features.samplerAnisotropy)
    {
        sampler.maxAnisotropy = vulkanDevice->properties.limits.maxSamplerAnisotropy;
        sampler.anisotropyEnable = VK_TRUE;
    }
    VK_CHECK_RESULT(vkCreateSampler(device, &sampler, NULL, &samplers[2]));

#endif

    // Create image view
    VkImageViewCreateInfo view;
    memset(&view, 0, sizeof(VkImageView));
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = vulkanTex->image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = GY_VK_TEXTURE_FORMAT_DEFAULT;
    view.components.r   = VK_COMPONENT_SWIZZLE_R;
    view.components.g   = VK_COMPONENT_SWIZZLE_G;
    view.components.b   = VK_COMPONENT_SWIZZLE_B;
    view.components.a   = VK_COMPONENT_SWIZZLE_A;
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 1;
    view.subresourceRange.levelCount = vulkanTex->mipLevels;
    if(vkCreateImageView(_vkDevice, &view, NULL, &vulkanTex->view) != VK_SUCCESS) _gyLog(GY_DEBUG_ERROR, "FUCK ME!!!");

    return;

destroy_device_mem:
    vkFreeMemory(_vkDevice, vulkanTex->deviceMemory, NULL);
destroy_image:
    vkDestroyImage(_vkDevice, vulkanTex->image, NULL);
destroy_buffer:
    gyVkResourceDestroyBuffer(&stagingBuffer);
}


int _gyVidTexFree(GYTexture *const tex) {
    VulkanTexture* vkTex;
    if(tex && (vkTex = (VulkanTexture*)tex->_data)) {
        vkDestroyImageView(_vkDevice, vkTex->view, NULL);
        vkDestroyImage(_vkDevice, vkTex->image, NULL);
        vkFreeMemory(_vkDevice, vkTex->deviceMemory, NULL);
        free(vkTex);
        tex->_data = NULL;
        return 1;
    } else return 0;
}
