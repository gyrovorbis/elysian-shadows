#include <gyro_input_api.h>
#include <kos.h>

int gyInputInit(const struct GYInputInitParams*const params) {
	return 1;
}

int gyInputUninit() {
	return 1;
}
void gyInputDeviceDump(void) {

}

static void _SetButDown(GYInput *const input, const GY_DC_BUTTON but) {
	input->butTapped[but] = !input->butDown[but];
        input->butDown[but] = 1;
}


static void _SetButUp(GYInput *const input, const GY_DC_BUTTON but) {
        input->butDown[but] = 0;
        input->butTapped[but] = 0;
}

void  gyInputPoll(struct GYInput** const input) {
	maple_device_t *cont;
	cont_state_t *state;

	cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
	state = (cont_state_t *)maple_dev_status(cont);

	if(state->buttons & CONT_DPAD_LEFT) _SetButDown(input, GY_CONT_DPAD_L);
	else _SetButUp(input, GY_CONT_DPAD_L);

	if(state->buttons & CONT_DPAD_RIGHT) _SetButDown(input, GY_CONT_DPAD_R);
	else _SetButUp(input, GY_CONT_DPAD_R);

	if(state->buttons & CONT_DPAD_UP) _SetButDown(input, GY_CONT_DPAD_U);   
	else _SetButUp(input, GY_CONT_DPAD_U);

	if(state->buttons & CONT_DPAD_DOWN) _SetButDown(input, GY_CONT_DPAD_D);
	else _SetButUp(input, GY_CONT_DPAD_D);

	if(state->buttons & CONT_B) _SetButDown(input, GY_CONT_BUT_B);
	else _SetButUp(input, GY_CONT_BUT_B);

	if(state->buttons & CONT_A) _SetButDown(input, GY_CONT_BUT_A);
	else _SetButUp(input, GY_CONT_BUT_A);

	if(state->buttons & CONT_Y) _SetButDown(input, GY_CONT_BUT_Y);
	else _SetButUp(input, GY_CONT_BUT_Y);

	if(state->buttons & CONT_X) _SetButDown(input, GY_CONT_BUT_X);
	else _SetButUp(input, GY_CONT_BUT_X);
	
	if(state->buttons & CONT_START) _SetButDown(input, GY_CONT_BUT_START);
	else _SetButUp(input, GY_CONT_BUT_START);

//	input->analog1X = state->joyx;
//	input->analog1Y = state->joyy;

	unsigned short int ltrig = state->ltrig; 
	unsigned short int rtrig = state->rtrig;

	if(ltrig > 100) _SetButDown(input, GY_CONT_LTRIG);
	else _SetButUp(input, GY_CONT_LTRIG);

	if(rtrig > 100) _SetButDown(input, GY_CONT_RTRIG);
	else _SetButUp(input, GY_CONT_RTRIG);
}

int gyKeyboardPoll(GYKeyboard**const keybd) {
    *keybd = NULL;
    return 1;
}

int gyMousePoll(GYMouse**const mouse) {
    *mouse = NULL;
    return 1;
}

int gyControllerPoll(const unsigned slot, GYController**const controller) {
    *controller = NULL;
    return 1;
}
