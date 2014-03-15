#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gyro_file_api.h>

/*
 * Should probably pull these defines out to somewhere more centralized...
 */
#if defined(_WIN32) || defined(_WIN64)
#   define TARGET_OS_WINDOWS
#elif defined(__ANDROID__) || defined(ANDROID)
#   define TARGET_OS_ANDROID
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__) || defined(__CYGWIN__)
#   define TARGET_OS_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
#   define TARGET_OS_MACOSX
#endif

#if !defined(TARGET_OS_WINDOWS)
#   include <limits.h> /* For PATH_MAX */
#endif

#ifdef TARGET_OS_MACOSX
extern char **NXArgv;
#endif

static char *gy__author = NULL;
static char *gy__name = NULL;

static char *gy__strdup(const char *str) {
    size_t len = 1 + strlen(str);
    char *p = malloc(len);
    return p ? memcpy(p, str, len) : NULL;
}

int gyFileInit(const char *author, const char *name)
{
    if (author == NULL || name == NULL) {
        /* Require an app author & name to identify us */
        return 0;
    }
    gy__author = gy__strdup(author);
    gy__name = gy__strdup(name);
    return 1;
}

int gyFileQuit(void)
{
    if (gy__author != NULL) {
        free(gy__author);
        gy__author = NULL;
    }
    if (gy__name != NULL) {
        free(gy__name);
        gy__name = NULL;
    }
    return 1;
}

static int gy__get_binary_path(char **path, size_t *len)
{
    char temp_path[2048];
    char *ptr;
    memset(temp_path, 0, sizeof temp_path);
#if defined(TARGET_OS_WINDOWS)
    GetModuleFileNameA(NULL, temp_path, sizeof temp_path);
    if (strlen(temp_path)) {
        ptr = &temp_path[strlen(temp_path)];
        while (*(--ptr) != '\\');
        ptr[1] = '\x0';
    } else {
        strcpy(temp_path, ".\\");
    }
#elif defined(TARGET_OS_LINUX)
    readlink("/proc/self/exe", temp_path, (sizeof temp_path) - 1);
    if (strlen(temp_path)) {
        ptr = &temp_path[strlen(temp_path)];
        while (*(--ptr) != '/');
        ptr[1] = '\x0';
    } else {
        strcpy(temp_path, "./");
    }
#elif defined(TARGET_OS_MACOSX)
    strncpy(temp_path, NXArgv[0], sizeof temp_path);
    if (strlen(temp_path)) {
        ptr = &temp_path[strlen(temp_path)];
        while (*(--ptr) != '/');
        ptr[1] = '\x0';
    } else {
        strcpy(temp_path, "./");
    }
#endif
#if defined(TARGET_OS_WINDOWS)
    *path = gy__strdup(temp_path);
#else
    *path = realpath(temp_path, NULL);
#endif
    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(*path);
    }
    return 1;
}

static int gy__get_userdata_path(char **path, size_t *len)
{
    char temp_path[2048];
#if defined(TARGET_OS_LINUX) || defined(TARGET_OS_MACOSX)
    const char *home_path;
#endif
    if (gy__author == NULL || gy__name == NULL) {
        /* gyFileInit(...) wasn't called */
        return 0;
    }
    memset(temp_path, 0, sizeof temp_path);
#if defined(TARGET_OS_WINDOWS)
    SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, temp_path);
    sprintf(temp_path, "%s\\%s\\%s\\", temp_path, gy__author, gy__name);
#elif defined(TARGET_OS_LINUX)
    home_path = getenv("HOME");
    sprintf(temp_path, "%s/.local/share/%s/%s", home_path, gy__author, gy__name);
#elif defined(TARGET_OS_MACOSX)
    home_path = getenv("HOME");
    sprintf(temp_path, "%s/Library/Application Support/%s/%s", home_path, gy__author, gy__name);
#endif
    *path = gy__strdup(temp_path);
    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(temp_path);
    }
    return 1;
}

static int gy__append_path_with_path(char **root, size_t *len, int (*base)(char **, size_t *), const char *path)
{
	char temp_path[2048];
#if !defined(TARGET_OS_WINDOWS)
	char real_path[PATH_MAX];
#endif
	if (!base(root, len)) {
		return 0;
	}
	memset(temp_path, 0, sizeof temp_path);
	sprintf(temp_path, "%s/%s", *root, path);
	free(*root);
#if defined(TARGET_OS_WINDOWS)
	*root = gy__strdup(temp_path);
#else
	realpath(temp_path, real_path);
	*root = gy__strdup(real_path);
#endif
	if (*root == NULL) {
		return 0;
	}
	if (len != NULL) {
		*len = strlen(*root);
	}
	return 1;
}

static int gy__get_assets_path(char **path, size_t *len)
{
	return gy__append_path_with_path(path, len, &gy__get_binary_path, GY_PATH_STR_ASSETS);
}

int gyFileResolvePath(GYPathType path_type, char **path, size_t *len)
{
    if (path == NULL) {
        return 0;
    }
    switch (path_type) {
        case GY_PATH_BINARY:
            return gy__get_binary_path(path, len);
        case GY_PATH_USERDATA:
            return gy__get_userdata_path(path, len);
		case GY_PATH_ASSETS:
			return gy__get_assets_path(path, len);
    }
    return 1;
}

int gyFileOpen(GYPathType path_type, const char *file_path, const char *mode, GYFile **fp)
{
    char full_path[2048];
    char *path_root;
    size_t path_root_len = 0;
    if (file_path == NULL || fp == NULL) {
        return 0;
    }
    if (!gyFileResolvePath(path_type, &path_root, &path_root_len)) {
        return 0;
    }
    memset(full_path, 0, sizeof full_path);
    sprintf(full_path, "%s/%s", path_root, file_path);
    *fp = malloc(sizeof(struct _GYFile));
    if (*fp == NULL) {
        return 0;
    }
    (*fp)->fp = fopen(full_path, mode);
    if ((*fp)->fp == NULL) {
        return 0;
    }
    free(path_root);
    return 1;
}

int gyFileRead(GYFile *fp, char *buffer, size_t count, size_t *bytes_read)
{
    size_t read_bytes = 0;
    if (fp == NULL || buffer == NULL) {
        return 0;
    }
    read_bytes = fread(buffer, sizeof(char), count, fp->fp);
    if (bytes_read != NULL) {
        *bytes_read = read_bytes;
    }
    return 1;
}

int gyFileSeek(GYFile *fp, off_t offset, int whence)
{
    if (fp == NULL || fp->fp == NULL) {
        return 0;
    }
    return !fseeko(fp->fp, offset, whence);
}

int gyFileLength(GYFile *fp, size_t *len)
{
    if (fp == NULL || fp->fp == NULL) {
        return 0;
    }
    fseek(fp->fp, 0, SEEK_END);
    *len = (size_t) ftell(fp->fp);
    rewind(fp->fp);
    return 1;
}

int gyFileTell(GYFile *fp, long int *pos)
{
    if (fp == NULL || fp->fp == NULL) {
        return 0;
    }
    *pos = ftell(fp->fp);
    return *pos > -1L;
}

int gyFileClose(GYFile **fp)
{
    if (fp == NULL || *fp == NULL) {
        return 0;
    }
    if (fclose((*fp)->fp)) {
        return 0;
    }
    free(*fp);
    *fp = NULL;
    return 1;
}
