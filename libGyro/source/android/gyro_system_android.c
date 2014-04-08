#include <gyro_system_api.h>

#include "../common/gyro_system_callbacks.h"

#include "android_native_app_glue.h"
#include "internal_android.h"

extern int main(int, char**);

struct android_app *gy_android_app;

void android_main(struct android_app *app)
{
	gy_android_app = app;
	main(0, 0);
}
