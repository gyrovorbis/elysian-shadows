#include <stdlib.h>
#include <math.h>
#include "gyro_tracking_internal.h"

GYTracking *_trackings = NULL;
unsigned int _num_trackings = 0;

static inline int custom_isnan(const double var) {
   volatile double d = var;
   return d != d;
}

char _IsInsideArea(GYTracking *track, int x, int y) {
    if(x >= track->x && y >= track->y && x <= track->x + track->width && y <= track->y + track->width)
        return 1;
    return 0;
}

char _IsMoveTriggered(GYMove *move, double angle) {
    if(custom_isnan(angle)) return 0;

    if(angle <= move->angle + move->tolerance && angle >= move->angle - move->tolerance)
        return 1;

    return 0;
}

GYTracking *gyTrackingCreate(unsigned int x, unsigned int y, unsigned int width, unsigned int height, int relative) {
    GYTracking *track = (GYTracking*)calloc(1, sizeof(GYTracking));
    if(!track) return NULL;

    track->x = x;
    track->y = y;
    track->width = width;
    track->height = height;
    track->relative = relative;

    track->next = _trackings;
    _trackings = track;
    _num_trackings++;
    return track;
}

GYTracking *_FindTrackParent(GYTracking *track) {
    GYTracking *temp = _trackings;

    while(temp->next != track)
        temp = (GYTracking*)temp->next;

    if(temp->next == track)
        return temp;

    return NULL;
}

void _DestroyMoves(GYMove *root) {
    if(!root) return;
    _DestroyMoves((GYMove*)root->next);
    free(root);
};

void gyTrackingDestroy(GYTracking *track) {
	GYTracking *parent = _FindTrackParent(track);
    if(!track) return;

    if(!parent) //track is root
        _trackings = (GYTracking*)track->next;
    else
        parent->next = (GYTracking*)track->next;
    _DestroyMoves(track->moves);
    free(track);
    --_num_trackings;
};

char gyTrackingAddMove(GYTracking *track, GYTrackingCallback cb, double angle, int tolerance, void *ud) {
    GYMove *move = (GYMove*)calloc(1, sizeof(GYMove));
	if(!track) return 1;
    if(!move) {
        return 1;
    }
    move->cb = cb;
    move->angle = angle;
    move->tolerance = tolerance;
    move->ud = ud;

    move->next = track->moves;
    track->moves = move;
    track->num_moves++;

    return 0;
}
