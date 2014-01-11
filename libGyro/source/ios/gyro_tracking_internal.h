#ifndef GYRO_TRACKING_INTERNAL_H
#define GYRO_TRACKING_INTERNAL_H

#include "gyro_tracking.h"

struct _GYMove
{
    GYTrackingCallback cb;
    double angle;
    int tolerance;
    void *ud;
    void *next;
};

struct _GYTracking
{
    GYMove *moves;
    int num_moves;
    int x, y;
    int width, height;
    int relative;
    double angle;
    void *next;
};

char _IsInsideArea(GYTracking *track, int x, int y);
char _IsMoveTriggered(GYMove *move, double angle);

extern GYTracking *_trackings;
extern unsigned int _num_trackings;

#endif

