#include "gyro_input.h"
#include "gyro_tracking.h"
#include <string.h>
#include "gyro_log.h"
GYInput _input;


GYTracking *tracking = NULL;

void gyInput_set_callback(int x, int y, int taps, void *ud, int set) {
	LogWrite("gyInput_set_callback: %d, %d, %d, %d, %d\n", x, y, taps, (int)ud, set);
	if(set)	{
		_input.keyDown[(int)ud] = 1;
		if(taps)
			_input.keyTapped[(int)ud] = 1;
	}
	else {
		_input.keyDown[(int)ud] = 0;
		_input.keyTapped[(int)ud] = 0;
	}
}

int gyInputInit(void) {
	LogWrite("gyInputInit\n");
	memset(&_input, 0, sizeof(GYInput));
	tracking = gyTrackingCreate(30, 100, 200, 200, 0);
	// todo add error checking
	gyTrackingAddMove(tracking, gyInput_set_callback, 180, 10, (void*)GY_KEY_DPAD_D);
	gyTrackingAddMove(tracking, gyInput_set_callback, 0, 10, (void*)GY_KEY_DPAD_U);
	gyTrackingAddMove(tracking, gyInput_set_callback, -90, 10, (void*)GY_KEY_DPAD_R);
	gyTrackingAddMove(tracking, gyInput_set_callback, 90, 10, (void*)GY_KEY_DPAD_L);
	return 0;
};

int gyInputUninit(void) {
	gyTrackingDestroy(tracking);
	return 0;
};

void gyInputPoll(GYInput *const input) {
	memcpy(input, &_input, sizeof(GYInput));
};
