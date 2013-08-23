#ifndef LIBGYRO_IOS_LOG_H
#define LIBGYRO_IOS_LOG_H

#import <stdarg.h>
void IOS_LogWrite(const char *s, va_list args);
void IOS_LogWrites(const char *s);
#endif

