#include <gyro_system_api.h>

#include "../common/gyro_system_callbacks.h"

#include "android_native_app_glue.h"

extern int main(int, char**);

void android_main(struct android_app *app)
{
	main(0, 0);
}
