#if 0
typedef struct _GYTexCoord2 {
#if GYRO_PLATFORM != GYRO_PLATFORM_DC
    float u, v;
#else
	uint32_t uv; //packed together on Dreamcast
#endif
} GYTexCoord2;

typedef struct _GYTexCoordFrame {
	GYTexCoord2 topLeft;
	GYTexCoord2 botRight;
#if GYRO_PLATFORM == GYRO_PLATFORM_DC
	GYTexCoord2 topRight;
	GYTexCoord2 botLeft;
#endif
	uint16_t spriteWidth, spriteHeight;
} GYTexCoordFrame;
#endif
