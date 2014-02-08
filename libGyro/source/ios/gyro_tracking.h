#ifndef GYRO_TRACKING_H
#define GYRO_TRACKING_H
//set = 1 then it's setting
typedef void (*GYTrackingCallback)(int x, int y, int taps, void *ud, int set);

struct _GYMove;
typedef struct _GYMove GYMove;

struct _GYTracking;
typedef struct _GYTracking GYTracking;

int gyTrackingInit();
void gyTrackingUninit();

/*
    Create new tracking area
*/
GYTracking *gyTrackingCreate(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int relative);

/*
    Destroy tracking area
*/
void gyTrackingDestroy(GYTracking *track);

/*
    Add new move event
*/
char gyTrackingAddMove(GYTracking *track, GYTrackingCallback cb, double angle, int tolerance, void *ud);

/*
    Translate a tracking area
*/
void gyTrackingTranslate(GYTracking *track, int x, int y);

/*
    Resize a tracking area
*/
void gyTrackingResize(GYTracking *track, int new_width, int new_height);

#endif
