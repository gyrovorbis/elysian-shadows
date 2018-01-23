#include <glal/gyro_gl_utils.h>

void glalGLDriverQuery(void) {
    GLint       glint[20];
    GLubyte*    glchar[100];
    GLfloat     glfloat[20];
    //GLboolean   glbool[20];

#define GL_QUERY(enumVal, strName, suffix, var, format) do {                \
        glGet##suffix(enumVal, &var);                                       \
        if(!GY_GL_CHECK_ERROR())                                            \
            _gyLog(GY_DEBUG_VERBOSE, "%-45s: %40"#format, strName,  var);   \
    } while(0);

#define GL_QUERY_INT(enumVal) \
    GL_QUERY(enumVal, #enumVal, Integerv, glint[0], d)

#define GL_QUERY_FLOAT(enumVal) \
    GL_QUERY(enumVal, #enumVal, Floatv, glfloat, f)

#define GL_QUERY_STRING(enumVal) do {                                               \
        const GLubyte* str = glGetString(enumVal);                                  \
        if(!str) GY_GL_CHECK_ERROR();                                               \
        else _gyLog(GY_DEBUG_VERBOSE, "%-45s: %40s", #enumVal, (const char*)str);   \
    } while(0);


    _gyLog(GY_DEBUG_VERBOSE, "Querying OpenGL Device Driver");
    _gyPush();
    glGetIntegerv(GL_MAJOR_VERSION, &glint[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glint[1]);
    if(!GY_GL_CHECK_ERROR()) {
        sprintf((char*)glchar, "%d.%d", glint[0], glint[1]);
        _gyLog(GY_DEBUG_VERBOSE, "%-45s: %40s", "GL Major.Minor Version", glchar);
    }

    GL_QUERY_STRING(GL_VERSION);
    GL_QUERY_STRING(GL_SHADING_LANGUAGE_VERSION);
    GL_QUERY_STRING(GL_VENDOR);

#ifdef GYRO_GLAL_GL_Desktop
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, glint);
    if(!GY_GL_CHECK_ERROR()) {
        _gyLog(GY_DEBUG_VERBOSE, "Context Properties");
        _gyPush();
        unsigned profileMask = (unsigned)glint[0];
        const char* profileStr = NULL;
        if(profileMask & GL_CONTEXT_CORE_PROFILE_BIT) {
            profileStr = "Core";

        } else if(profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
            profileStr = "Compatability";
        }
        _gyLog(GY_DEBUG_VERBOSE, "%-45s: %40s", "Profile Type", profileStr? profileStr : "None?");

        glGetIntegerv(GL_CONTEXT_FLAGS, glint);
        if(!GY_GL_CHECK_ERROR()) {
            unsigned flags = (unsigned)glint[0];
            _gyLog(GY_DEBUG_VERBOSE, "Flags");
            _gyPush();
            if(flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) _gyLog(GY_DEBUG_VERBOSE, "FORWARD_COMPATIBLE_BIT");
            if(flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) _gyLog(GY_DEBUG_VERBOSE, "DEBUG_BIT");
            if(flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) _gyLog(GY_DEBUG_VERBOSE, "ROBUST_ACCESS_BIT");
            if(flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) _gyLog(GY_DEBUG_VERBOSE, "NO_ERROR_BIT");
            _gyPop(1);
        }

        _gyPop(1);

    }
#endif

    _gyLog(GY_DEBUG_VERBOSE, "Limits");
    _gyPush();

    GL_QUERY_INT(GL_MAX_3D_TEXTURE_SIZE);
    GL_QUERY_INT(GL_MAX_ARRAY_TEXTURE_LAYERS);
    GL_QUERY_INT(GL_MAX_COLOR_ATTACHMENTS);
    GL_QUERY_INT(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS);
    GL_QUERY_INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    GL_QUERY_INT(GL_MAX_COMBINED_UNIFORM_BLOCKS);
    GL_QUERY_INT(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
    GL_QUERY_INT(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    GL_QUERY_INT(GL_MAX_DRAW_BUFFERS);
    //GL_QUERY_INT(GL_MAX_ELEMENT_INDEX);
    GL_QUERY_INT(GL_MAX_ELEMENTS_INDICES);
    GL_QUERY_INT(GL_MAX_ELEMENTS_VERTICES);
    GL_QUERY_INT(GL_MAX_FRAGMENT_INPUT_COMPONENTS);
    GL_QUERY_INT(GL_MAX_FRAGMENT_UNIFORM_BLOCKS);
    GL_QUERY_INT(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    GL_QUERY_INT(GL_MAX_FRAGMENT_UNIFORM_VECTORS);
    GL_QUERY_INT(GL_MAX_PROGRAM_TEXEL_OFFSET);
    GL_QUERY_INT(GL_MAX_RENDERBUFFER_SIZE);
    GL_QUERY_INT(GL_MAX_SAMPLES);
    GL_QUERY_INT(GL_MAX_SERVER_WAIT_TIMEOUT);
    GL_QUERY_INT(GL_MAX_TEXTURE_IMAGE_UNITS);
    GL_QUERY_INT(GL_MAX_TEXTURE_LOD_BIAS);
    GL_QUERY_INT(GL_MAX_TEXTURE_SIZE);
    GL_QUERY_INT(GL_MAX_UNIFORM_BLOCK_SIZE);
    GL_QUERY_INT(GL_MAX_UNIFORM_BUFFER_BINDINGS);
   // GL_QUERY_INT(GL_MAX_VARYING_COMPONENTS);
    GL_QUERY_INT(GL_MAX_VARYING_VECTORS);
    GL_QUERY_INT(GL_MAX_VERTEX_ATTRIBS);
    GL_QUERY_INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    GL_QUERY_INT(GL_MAX_VERTEX_OUTPUT_COMPONENTS);
    GL_QUERY_INT(GL_MAX_VERTEX_UNIFORM_BLOCKS);
    GL_QUERY_INT(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
    GL_QUERY_INT(GL_MAX_VERTEX_UNIFORM_VECTORS);
    GL_QUERY_INT(GL_MAX_VIEWPORT_DIMS);
    GL_QUERY_INT(GL_MIN_PROGRAM_TEXEL_OFFSET);

    _gyPop(1);

    GLint extCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
    if(!GY_GL_CHECK_ERROR()) {
        _gyLog(GY_DEBUG_VERBOSE, "Extensions [%d]", extCount);
        _gyPush();

        for(GLint i = 0; i < extCount; ++i) {
            const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
            if(GY_GL_CHECK_ERROR() || !ext) break;

            _gyLog(GY_DEBUG_VERBOSE, "[%d] %s", i, ext);
        }


        _gyPop(1);
    }

    _gyPop(1);

}
