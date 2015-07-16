#ifndef GYRO_INPUT_IOS_INL
#define GYRO_INPUT_IOS_INL
#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYInputInitParams {
    char compat;
} GYInputInitParams;


typedef struct GYInput {
    int up;
    int down;
    int left;
    int right;
} GYInput;

#ifdef __cplusplus
}
#endif

#endif // GYRO_INPUT_IOS_INL

