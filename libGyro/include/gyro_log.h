#ifndef LIBGYRO_LOG_H
#define LIBGYRO_LOG_H

#include <stdarg.h>
#include <stdlib.h>

int LogInit(const char *file);
void LogSetFilename(const char *file);
void LogWrite(const char *s, ...);
void LogWritev(const char *s, va_list args);
void LogWrites(const char *s);
void LogDestroy();

#endif

