#include <gyro_test_api.h>

typedef struct GYTestQueue {
    GYTest*     stack[GYRO_TEST_QUEUE_DEPTH_MAX];
    GYTest*     last;
    GYTest*     first;
    GYTest*     current;
    uint32_t    completeCount;
    uint32_t    pendingCount;
} GYTestQueue;


