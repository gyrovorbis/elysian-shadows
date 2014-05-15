#include <stdint.h>

#include <gyro_timer_api.h>

#include <sys/time.h>
#include <unistd.h>

static int64_t gy_init_time;

static uint64_t __get_raw_time(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_sec * (uint64_t) 1000000 + (uint64_t) tv.tv_usec;
}

int gyTimerInit(void) {
	gy_init_time = __get_raw_time();
    return 1;
}

unsigned long int gyTimerTicks(void)
{
	return (__get_raw_time() - gy_init_time) * 1e-3;
}

void gyTimerSleep(const unsigned long duration) {
    (void)duration;
    usleep(duration * 1000);
}
