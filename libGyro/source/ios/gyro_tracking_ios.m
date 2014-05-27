#include "ios_system.h"
#include "gyro_tracking_internal.h"
//#include "gyro_timer.h"
//#include "gyro_log.h"
#include <string.h>

typedef struct _Track {
    UITouch *id;
    unsigned long ticks;
    double angle;
    int taps;
    int x, y;
    GYTracking *current;
} Track;

Track tracks[10];
char walk[10] = {0,1,2,3,4,5,6,7,8,9};
char pivot = 0;

void Callbacks(int set) {
//    LogWrite("%d %d %d %d %d %d %d %d %d %d: %d\n", walk[0], walk[1], walk[2], walk[3],
//                                                    walk[4], walk[5], walk[6], walk[7],
//                                                    walk[8], walk[9], pivot);
//    GYMove *move;
//    int i;
//    for(i = 0; i < 10; i++) {
//        if(tracks[i].id == NULL) continue;
//        move = tracks[i].current->moves;
//        while(move) {
//            if(_IsMoveTriggered(move, tracks[i].angle))
//                move->cb(tracks[i].x, tracks[i].y, tracks[i].taps, move->ud, set);
//            move = (GYMove*)move->next;
//        }
//    }
}

void GetAngle(Track *track, GYTracking *gyt, void *ui, char first) {
//    CGPoint v1, v2;
//    if(gyt->relative) {
//        if(first) {
//            track->angle = NAN;
//            return;
//        }
//        v1 = [track->id previousLocationInView:(UIView*)ui];
//    }
//    else {
//        v1.x = (float)(gyt->x + (float)(gyt->width / 2.0f));
//        v1.y = (float)(gyt->y + (float)(gyt->height / 2.0f));
//    }
//    v2 = [track->id locationInView:(UIView*)ui];
//    track->x = v2.x;
//    track->y = v2.y;
//    track->angle = (atan2(v1.x - v2.x, v1.y - v2.y) * 180.0f / M_PI);
//    LogWrite("track: %u, %u, %f\n", track->x, track->y, track->angle);
}

Track *GetTrack(UITouch *touch) {
//    int i;
//    for(i = 0; i < 10; i++) {
//        if(tracks[i].id == touch)
//            return &tracks[i];
//    }
    return NULL;
}

char AddTrack(UITouch *id, unsigned long int ticks, GYTracking *track, void *ui) {
//    LogWrite("AddTrack\n");
//    if(GetTrack(id)) return 0;
//    Track *t = NULL;
//    if(pivot == 10) return 0;
//
//    char temp_pivot = pivot;
//    pivot = walk[(int)pivot];
//    walk[(int)temp_pivot] = temp_pivot;
//
//    t = &tracks[(int)pivot];
//    if(walk[(int)pivot] == pivot)
//        pivot++;
//    memset(t, 0, sizeof(Track));
//
//    t->id = id;
//    t->ticks = ticks;
//    t->taps = [id tapCount];
//    t->current = track;
//    GetAngle(t, track, ui, 1);
    return 1;
}

void VoidTrack(Track *track) {
//    if(!track) return;
//    unsigned int lookup = ((char*)track - (char*)tracks) / sizeof(Track);
//    memset(&tracks[lookup], 0, sizeof(Track));
//    if(pivot == 10) pivot--;
//    if(pivot == lookup) {
//        pivot = walk[(int)pivot];
//        return;
//    }
//    walk[lookup] = walk[(int)pivot];
//    walk[(int)pivot] = lookup;
}

void IOS_TouchesBegan(void *set, void *ui) {
//    LogWrite("touches begin!\n");
//    CGPoint p;
//    GYTracking *temp = _trackings;
//    for(UITouch *touch in (NSSet*)set) {
//        p = [touch locationInView:(UIView*)ui];
//        while(temp) {
//            if(_IsInsideArea(temp, p.x, p.y)) {
//                AddTrack(touch, gyTimerTicks(), temp, ui);
//                break;
//            }
//            temp = temp->next;
//        }
//    }
    Callbacks(1);
}

void IOS_TouchesMoved(void *set, void *ui) {
//    LogWrite("touches moved\n");
//    Track *track = NULL;
//    for(UITouch *touch in (NSSet*)set) {
//        if((track = GetTrack(touch))) {
//            GetAngle(track, track->current, ui, false);
//            track->taps = [touch tapCount];
//        }
//    }
    Callbacks(1);
}

void IOS_TouchesEnded(void *set, void *ui) {
//    LogWrite("touches ended\n");
    Callbacks(0);
//    for(UITouch *touch in (NSSet*)set) {
//        VoidTrack(GetTrack(touch));
//    }
}

void IOS_TouchesCancelled() {
//    int i = 0;
//    for(i = 0; i < 10; i++) {
//        walk[i] = i;
//    }
//    memset(tracks, 0, sizeof(Track) * 10);
}

int gyTrackingInit() {
//    LogWrite("gyTracking Init!");
//    memset(tracks, 0, sizeof(Track) * 10);
    return 1;
}

void gyTrackingUninit() {
}
