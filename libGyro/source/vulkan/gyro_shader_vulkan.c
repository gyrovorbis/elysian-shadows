#include <vulkan/gyro_descriptor_vulkan.h>
#include <vulkan/gyro_video_vulkan.h>
#include <vulkan/gyro_context_vulkan.h>
#include <vulkan/gyro_util_vulkan.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>
#include <vulkan/gyro_vulkan.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



typedef struct GYVkPipelineLayout {
    VkPipelineLayout            pipelineLayout;
   // uint32_t                    setLayoutsCount;
   // GYVkDescriptorSetLayout*    setLayouts;
   // uint32_t                    pushConstantRangeCount;
   // VkPushConstantRange*        pushConstantRanges;
} GYVkPipelineLayout;


typedef struct GYVkShader {
    char                path[GYRO_PATH_MAX_SIZE];
    VkShaderModule*     shaderModule;
    GYVkPipelineLayout* pipelineLayout;
    VkCommandBuffer     commandBuffer;
    //actual data for shit!
} GYVkShader;




typedef struct GYVkPipeline {
    VkPipelineCache     pipelineCache;
    VkPipeline          pipeline;
    uint32_t            descriptorSetsCount;
    GYVkDescriptorSet*  descriptorSets;
} GYVkPipeline;



int gyVkPipelineLayoutCreate(GYVkPipelineLayout* layout, uint32_t setLayoutCount, const GYVkDescriptorSetLayout** setLayouts, uint32_t pushConstantRangeCount, const VkPushConstantRange* pushConstantRanges) {
    assert(layout && setLayoutCount && setLayouts);

    VkDescriptorSetLayout vkLayouts[setLayoutCount];

    for(unsigned i = 0; i < setLayoutCount; ++i) vkLayouts[i] = setLayouts[i]->descriptorSetLayout;

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        NULL,
        0, //flags, not used by API yet
        setLayoutCount,
        vkLayouts,
        pushConstantRangeCount,
        pushConstantRanges
    };

    return GY_VK_VERIFY(vkCreatePipelineLayout(_vkDevice, &pipelineLayoutInfo, NULL, &layout->pipelineLayout));
}

void gyVkPipelineLayoutDestroy(GYVkPipelineLayout* layout) {
    assert(layout);
    vkDestroyPipelineLayout(_vkDevice, layout->pipelineLayout, NULL);
}



#if 0

int InitializeRenderPass() {
    GYVkDescriptor descriptorsSet1[COUNT] = {


    };
    GYVkDescriptor descriptorsSet2[COUNT] = {


    };
    GYVkDescriptorSetLayout layouts[SETS] = {
        gyVkDescriptorSetLayoutCreate(0, NUM_DESCRIPTORS, &descriptorsSet1),
        gyVkDescriptorSetLayoutCreate(1, NUM_DESCRIPTORS, &descriptorsSet2)
    };
    GYVkPipelineLayout* gyVkPipelineLayoutCreate(NUM_DESCRIPTOR_SETS, &layouts);

    GYVkShader* shader = gyVkShaderCreate("blah.glsl", "cache.bin");


    gyVkDescriptorBufferCreate(shader, 0, 1, sizeof(integer), /*BUFF CREATE PARAMS!?*/);
    gyVkDescriptorImageCreate(shader, 0, 2, /*IMG CREATE PARAMS!?!*/);
    gyVkDescriptorBufferUpdate(shader, 0, 1, &value);

    //load pipeline cache

    //create descriptor pool

    //create descriptor sets + populate

    //create cmd buffer

    //bind descriptor sets to cmd buffer

}




int _gyVidShaderCreate(_GYVkShader* shader) {

    VkShaderModuleCreateInfo shaderCreateInfo = {
        STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        NULL,
        0,  //ShaderModuleCreateFlags
        0, //code size
        NULL, //code
    };

    GY_VK_VERIFY(vkCreateShaderModule(_vkDevice,
                         &shaderCreateInfo,
                         NULL,                  //allocator
                         &shader->shaderModule));



}

int _gyVidShaderDestroy(_GYVkShader* shader) {
    vkDestroyShaderModule(_vkDevice,
                          shader->shaderModule,
                          NULL); //allocator
}





#endif



