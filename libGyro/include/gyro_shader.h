#ifndef GYRO_SHADER_H
#define GYRO_SHADER_H

#include "gyro_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYShader {
	char	path[GYRO_PATH_MAX_SIZE];
	int		type;
	int		shader;
} GYShader;

typedef struct _GYShaderProgram {
	char		path[GYRO_PATH_MAX_SIZE];
	GYShader*	vertShader;
	GYShader*	fragShader;
	int			program;
} GYShaderProgram;

GYShader* gyShaderCreate(const char*const path);
int gyShaderDestroy(GYShader* shader);

GYShaderProgram* gyShaderProgramCreate(GYShader* vertShader, GYShader* fragShader);
int gyShaderDestory(GYShaderProgram* program);

int gyShadersReloadAll(void);
void gyShaderWalk(void);




#ifdef __cplusplus
}
#endif


#endif // GYRO_SHADER_H
