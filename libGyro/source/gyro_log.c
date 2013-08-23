#include "gyro_platform.h"
#include "gyro_log.h"
#if GYRO_PLATFORM == GYRO_IOS
	#include "ios_log.h"
#else
	#include <stdio.h>
	FILE *f = NULL;
#endif

const char *default_log = "debug.txt";
int file_open = 0;

int LogInit(const char *file) {
#if GYRO_PLATFORM != GYRO_IOS
	if(file_open)
		LogDestroy();

	if(file)
		f = fopen(file, "w");
	else
		f = fopen(default_log, "w");

	if(!f)
		return 0;
#endif
	return (file_open = 1);
}

void LogSetFilename(const char *file) {
	if(file_open)
		LogDestroy();

	LogInit(file);
}

void LogDestroy() {
#if GYRO_PLATFORM != GYRO_IOS
	if(file_open) {
		file_open = 0;
		fclose(f);
		f = NULL;
	}
#endif
}

void LogWritev(const char *s, va_list args) {
#if GYRO_PLATFORM == GYRO_IOS
	IOS_LogWrite(s, args);
#else
	vprintf(s, args); //stdout
	fflush(stdout);
	if(file_open) {
		vfprintf(f, s, args); //file
		fflush(f);
	}
#endif
}

void LogWrite(const char *s, ...) {
	va_list args;
	va_start(args, s);
	LogWritev(s, args);
	va_end(args);
}

void LogWrites(const char *s) {
#if GYRO_PLATFORM == GYRO_IOS
	IOS_LogWrites(s);
#else
	puts(s); //stdout
	fflush(stdout);
	if(file_open) {
		fputs(s, f); //file
		fflush(f);
	}
#endif
}
