#include <gyro_shader_api.h>
#include "gyro_asset_bank.h"
#include <gyro_video_api.h>
#include <string.h>
#include <stdlib.h>

typedef struct __ShaderEntryData {
	const char* path;
} _ShaderEntryData;


typedef struct __ShaderProgramEntryData {
	GYShader* vertShader;
	GYShader* fragShader;
} _ShaderProgramEntryData;

//==== INTERNAL SHADER BANK ====


static int _ShaderComparator(const void *const payload, const void *const data) {
    if(!payload) return 0;
	return (strcmp(((GYShader*)payload)->path, ((_ShaderEntryData*)data)->path) == 0);
}

static void *_ShaderConstructor(void *const data) {
	GYShader *shader = malloc(sizeof(GYShader));

	strcpy(shader->path, ((_ShaderEntryData*)data)->path);
	if(!_gyVidShaderCreate(shader)) {
		return 0; //shit has reested
	}

	return shader;
}

static int _ShaderDestructor(void *const payload) {
	_gyVidShaderDestroy(payload);
	free(payload);
	return 1;
}

static GYAssetBank _shaderBank = { 0, 0, 0, &_ShaderComparator, &_ShaderConstructor, &_ShaderDestructor };

//==== INTERNAL SHADER PROGRAM BANK ====

static int _ShaderProgramComparator(const void *const payload, const void *const data) {
	return (((GYShaderProgram*)payload)->vertShader == ((_ShaderProgramEntryData*)data)->vertShader &&
			((GYShaderProgram*)payload)->fragShader == ((_ShaderProgramEntryData*)data)->fragShader);
}

static void *_ShaderProgramConstructor(void *const data) {
	GYShaderProgram *program = malloc(sizeof(GYShaderProgram));
	_ShaderProgramEntryData *entry = data;

	program->vertShader = entry->vertShader;
	program->fragShader = entry->fragShader;

	if(!_gyVidShaderProgramCreate(program)) {
		return 0; //shit has reested
	}

	return program;
}

static int _ShaderProgramDestructor(void *const payload) {
	_gyVidShaderProgramDestroy(payload);
	free(payload);
	return 1;
}

static GYAssetBank _shaderProgramBank = { 0, 0, 0, &_ShaderProgramComparator, &_ShaderProgramConstructor, &_ShaderProgramDestructor };


// ==== OTHER PUBLIC API ====

void gyShaderWalk(void);


/*I hate to fucking do this, but SDL forces us with window resizing...
	ughhghg --Falco */
#if 0
int gyShadersReloadAll(void) {
	unsigned int success = 1;

	//Destroy all programs
	GYAssetBankEntry *entry = _shaderProgramBank._head;
	GYShaderProgram *program;
	for(; entry; entry = entry->_next) {
		program = entry->_payload;
		if(!program) continue;

		success &= _gyVidShaderProgramDestroy(program);
	}


	//Destroy and reload all shaders
	entry = _shaderBank._head;
	GYShader *shader;
	for(; entry; entry = entry->_next) {
		shader = entry->_payload;
		if(!shader) continue;

		success &= _gyVidShaderDestroy(shader);
		success &= _gyVidShaderCreate(shader);

	}

	//Reload all programs
	entry = _shaderProgramBank._head;
	for(; entry; entry = entry->_next) {
		program = entry->_payload;
		if(!program) continue;

		success &= _gyVidShaderProgramCreate(program);

	}

	return success;

}
#endif


//==== PUBLIC API ====

GYShader* gyShaderCreate(const char*const path) {
	_ShaderEntryData data = { path };
	return gyAssetBankInsert(&_shaderBank, &data);
}

int gyShaderDestroy(GYShader* shader) {
	if(!shader) return 0;
	_ShaderEntryData data = { shader->path };
	return gyAssetBankRemove(&_shaderBank, &data);
}

GYShaderProgram* gyShaderProgramCreate(GYShader* vertShader, GYShader* fragShader) {
	_ShaderProgramEntryData data = { vertShader, fragShader };
	return gyAssetBankInsert(&_shaderProgramBank, &data);
}

int gyShaderProgramDestroy(GYShaderProgram* shader) {
    if(!shader) return 0;
    _ShaderProgramEntryData data = { shader->vertShader, shader->fragShader };
    return gyAssetBankRemove(&_shaderProgramBank, &data);
}

int gyShaderDestory(GYShaderProgram* program) {
	_ShaderProgramEntryData data = { program->vertShader, program->fragShader };
	return gyAssetBankRemove(&_shaderProgramBank, &data);
}
