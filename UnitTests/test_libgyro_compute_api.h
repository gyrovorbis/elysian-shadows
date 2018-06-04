#ifndef TEST_COMPUTE_SYSTEM_H
#define TEST_COMPUTE_SYSTEM_H

#include <QDebug>
#include <QtTest>
#include <gyro_compute_api.h>

namespace tktest {

#define TEST_LIBGYRO_COMPUTE_API_TEST_KERNEL_PATH   "/assets/opencl/particle_vbo.cl"
#define TEST_LIBGYRO_COMPUTE_API_TEST_KERNEL_NAME   "PopulateParticleVbo"

class LibGyroComputeApiTest: public QObject {
    Q_OBJECT
private:
    const GYComputeInitParams _gyInitParams = {
        0
    };

    unsigned                _platformCount;
    GYComputePlatformInfo*  _platformInfo;
    GYComputeDevice*        _device;
    GYComputeDeviceInfo*    _deviceInfo;
    GYComputeContext*       _context;
    GYComputeCommandQueue*  _queue;
    GYComputeProgram*       _program;
    GYComputeKernel*        _kernel;




private Q_SLOTS:
    void initialize(void);
    void uninitialize(void);
    void reinitialize(void);

    void platformQueryInfo(void);


    void reuninitialize(void);

};

inline void LibGyroComputeApiTest::initialize(void) {
    QVERIFY(gyComputeInit(&_gyInitParams));
}

inline void LibGyroComputeApiTest::uninitialize(void) {
    QVERIFY(gyComputeUninit());
}

inline void LibGyroComputeApiTest::reinitialize(void) {

}

inline void LibGyroComputeApiTest::platformQueryInfo(void) {


}

inline void LibGyroComputeApiTest::reuninitialize(void) {

}

}
#endif // TEST_COMPUTE_SYSTEM_H
