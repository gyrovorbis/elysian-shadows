#ifndef GYRO_TEST_API_H
#define GYRO_TEST_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_TEST_NAME_SIZE         200
#define GYRO_TEST_ERROR_MSG_SIZE    200
#define GYRO_TEST_QUEUE_DEPTH_MAX   10

typedef struct GYTestInitParams {


} GYTestInitParams;

typedef void (*GYTestCallback)(void*);

typedef struct GYTestCallbacks {
    GYTestCallback init;
    GYTestCallback deinit;
    GYTestCallback run;
} GYTestCallbacks;

typedef enum GY_TEST_STATE {
    GY_TEST_INACTIVE,
    GY_TEST_PENDING,
    GY_TEST_RUNNING,
    GY_TEST_PASSED,
    GY_TEST_FAILED,
    GY_TEST_SKIPPED
} GY_TEST_STATE;

typedef enum GY_TEST_FLAGS {
    GY_TEST_FLAG_NONE = 0x0,
    GY_TEST_FLAG_FORK = 0x1,
} GY_TEST_FLAGS;

typedef struct GYTest {
    char                name[GYRO_TEST_NAME_SIZE];
    char                errorMsg[GYRO_TEST_ERROR_MSG_SIZE];

    GYTestCallbacks     callbacks;
    struct GYTest*      parent;
    struct GYTest*      next;

    GY_TEST_STATE       state;
    uint32_t            ticks;
    uint32_t            maxTicks;
    uint32_t            flags;
/* LATER!
    uint32_t            heapAllocations;
    uint32_t            heapDeallocations;
    uint32_t            vramAllocations;
    uint32_t            luaBytes;
*/
} GYTest;

//API init/deinit
int         gyTestInit(const GYTestInitParams* params);
int         gyTestUninit(void);

//Test creation/destruction
GYTest*     gyTestCreate(const char* name, const GYTestCallbacks* callbacks);
GYTest*     gyTestCreateSimple(const char* name, GYTestCallback runCallback);
int         gyTestDestroy(GYTest* test);

//Queue Management
int         gyTestEnqueue(struct GYTest* test);
int         gyTestRemove(struct GYTest* test);
int         gyTestDisable(struct GYTest* test);
int         gyTestClearQueue(void);

//Parenting stack management
int         gyTestPush(struct GYTest* test);
int         gyTestPop(unsigned depth);

//Stats
unsigned    gyTestPendingCount(void);
unsigned    gyTestCompletedCount(void);
void        gyTestUpdateQueue(void);

//Test management
void gyTestFail(const char* message);
void gyTestSkip(const char* message);
void GY_VERIFY(int boolean, const char* message); //optional msg




#ifdef __cplusplus
}
#endif

#endif // GYRO_TEST_API_H
