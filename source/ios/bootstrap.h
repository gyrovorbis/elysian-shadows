#ifndef LIBGYRO_IOS_BOOTSTRAP_H
#define LIBGYRO_IOS_BOOTSTRAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*_RenderFunc)(void);
typedef int (*_UpdateFunc)(void);
typedef void (*_InitFunc)(int argc, char *argv[]);

void IOS_SetInitRenderFunc(_RenderFunc func);
void IOS_CallInitRenderFunc(int width, int height);

void IOS_SetInitFunc(_InitFunc func);
void IOS_CallInitFunc(void);


void IOS_CallRenderFunc(void);


void IOS_CallUpdateFunc(void);

int IOS_StartUpdate(void);


void IOS_SetRenderFunc(_RenderFunc func);
void IOS_SetUpdateFunc(_UpdateFunc func);
int CallUIApplicationMain(int argc, char *argv[]);
    
#ifdef __cplusplus
}
#endif
    
#endif
