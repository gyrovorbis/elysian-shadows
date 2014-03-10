#include "gyro_timer.h"
#include <sys/time.h>

static struct timeval start, now;

int gyTimerInit() {
	gettimeofday(&start, NULL);
	return 0;
}

unsigned long int gyTimerTicks() {
	gettimeofday(&now, NULL);
	return ((now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000);
}
