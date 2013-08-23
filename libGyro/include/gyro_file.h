#ifndef GYRO_FILE_H
#define GYRO_FILE_H

/* You can thank iOS for this reesty-ass API. 
 * Since you can never know where the app bundle resides,
 * you must retrieve every file manually from the bundle.
 * This means any file accesses within libGyro must use this
 * wrapper.  -- Gyro
 */

#ifdef __cplusplus
extern "C" {
#endif

    
#if GYRO_PLATFORM == GYRO_PLATFORM_iOS    
    
int gyFileInit(void);
const char* gyFilePath(const char *const file);   
    
#else 
    
//Dummy implementation for every other platform without a cock in its ass.
inline static int gyFileInit(void) { return 1; }
inline static const char* gyFilePath(const char *const file) { return file; } 

#endif
    
#ifdef __cplusplus
}
#endif

#endif
