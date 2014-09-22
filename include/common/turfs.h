#ifndef __included_turfs_h
#define __included_turfs_h

/* Needed for size_t and off_t */
#include <sys/types.h>

typedef enum {
    /*
     * Path to application (read-only) assets,
     * typically relative to the executing
     * binary's directory.
     *
     * Default virtual path: /assets
     */
    TURFS_PATH_ASSETS,

    /*
     * Path to data stored by the application
     * after it's installed, like user saves
     * and preferences.
     *
     * Default virtual path: /userdata
     */
    TURFS_PATH_USERDATA
} turfs_path_type;

/*
 * turfs return codes
 */
typedef enum {
    TURFS_RET_OK = 0,
    TURFS_RET_END_OF_FILE,
    TURFS_RET_INVALID_ARGS,
    TURFS_RET_MEMORY_ERROR,
    TURFS_RET_NOT_INITIALIZED,
    TURFS_RET_NOT_FOUND,
    TURFS_RET_UNKNOWN
} turfs_ret;

/**
 * turfs File pointer type
 */
typedef struct turfs_fp * turfs_fp;

#define TURFSAPI turfs_ret

/**
 * Initialize turfs
 *
 * This must be called prior to doing anything else with
 * this library.
 *
 * @param[in]   author      Author of the application, no special characters
 * @param[in]   name        Name of the application, no special characters
 * @param[in]   aactivity   If on Android, pass the ANativeActivity provided
 *                          by android_main()'s struct android_app parameter;
 *                          Otherwise, pass NULL.
 *
 * @returns TURFS_RET_OK if initialization succeeded.
 */
TURFSAPI turfs_init(const char *author, const char *name, void *aactivity);

/**
 * De-initialize turfs
 *
 * This should be called when you are finished using
 * this library in your application.
 *
 * @returns TURFS_RET_OK if turfs quit successfully, or
 * TURFS_RET_NOT_INITIALIZED if turfs_init had not yet been
 * called.
 */
TURFSAPI turfs_quit(void);

/**
 * Resolves a turfs_path_type to a virtual path
 *
 * @param[in]   type    Path type to resolve virtually
 * @param[out]  path    Location where path will be stored
 * @param[out]  len     Location where path length will be stored
 */
TURFSAPI turfs_virtual_path(turfs_path_type type, char **path, size_t *len);

/**
 * Resolves a turfs_path_type to its real system path
 *
 * @param[in]   type    Path type to resolve
 * @param[out]  path    Location where path will be stored
 * @param[out]  len     Location where path length will be stored
 */
TURFSAPI turfs_resolve_path(turfs_path_type type, char **path, size_t *len);

/**
 * Translates a full virtual path to its real system path
 *
 * @param[in]   vpath   Full virtual path
 * @param[out]  path    Location where real path will be stored
 * @param[out]  len     Location where real path length will be stored
 * @param[out]  type    Location where virtual path type will be stored
 */
TURFSAPI turfs_silph_scope(const char *vpath, char **path, size_t *len, turfs_path_type *type);

/**
 * Opens a file given an absolute virtual path.
 *
 * @param[in]   file    Absolute virtual path to file
 * @param[in]   mode    C string containing file access mode
 * @param[out]  fp      File pointer
 *
 * @returns TURFS_RET_OK if the file was opened successfully,
 * TURFS_RET_NOT_INITIALIZED if this function is called prior
 * to turfs_init, TURFS_RET_NOT_FOUND if the specified file
 * does not exist, or TURFS_RET_UNKNOWN if the file could not
 * be opened for some unspecified reason.
 */
TURFSAPI turfs_file_open(const char *file, const char *mode, turfs_fp *fp);

/**
 * Reads bytes from an opened file
 *
 * @param[in]   fp          File to read from
 * @param[in]   buffer      Buffer to read to
 * @param[in]   count       Number of bytes to read
 * @param[out]  bytes_read  Number of bytes read
 */
TURFSAPI turfs_file_read(turfs_fp fp, void *buffer, size_t count, size_t *bytes_read);

/**
 * Seeks to a position in a file. Equivalent to fseek()
 *
 * @param[in]   fp
 * @param[in]   offset
 * @param[in]   whence
 */
TURFSAPI turfs_file_seek(turfs_fp fp, off_t offset, int whence);

/**
 * Fetches the length of an open file.
 *
 * @param[in]   fp  File to fetch length of
 * @param[out]  len Location to store length
 */
TURFSAPI turfs_file_length(turfs_fp fp, size_t *len);

/**
 * Fetches the current position within a file.
 * Equivalent to ftell().
 *
 * @param[in]   fp  File to fetch position of
 * @param[out]  pos Location to store position
 */
TURFSAPI turfs_file_tell(turfs_fp fp, long int *pos);

/**
 * Test for end-of-file
 *
 * @returns TURFS_RET_END_OF_FILE if EOF was reached,
 * or TURFS_RET_OK otherwise.
 */
TURFSAPI turfs_file_eof(turfs_fp fp);

/**
 * Close an open file
 *
 * @param[in]   fp  Pointer to file to be closed
 */
TURFSAPI turfs_file_close(turfs_fp *fp);

#endif /* __included_turfs_h */
