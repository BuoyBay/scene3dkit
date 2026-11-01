#ifndef SCENE3DKIT_PLATFORM_H
#define SCENE3DKIT_PLATFORM_H
/* All operations, including destruction, belong to the creating thread.
 * Contexts do not share GPU resources. No visible window is created. */
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
extern PFNGLGENRENDERBUFFERSPROC sg_glGenRenderbuffers;
#define glGenRenderbuffers sg_glGenRenderbuffers
extern PFNGLBINDRENDERBUFFERPROC sg_glBindRenderbuffer;
#define glBindRenderbuffer sg_glBindRenderbuffer
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC sg_glRenderbufferStorageMultisample;
#define glRenderbufferStorageMultisample sg_glRenderbufferStorageMultisample
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC sg_glFramebufferRenderbuffer;
#define glFramebufferRenderbuffer sg_glFramebufferRenderbuffer
extern PFNGLDELETERENDERBUFFERSPROC sg_glDeleteRenderbuffers;
#define glDeleteRenderbuffers sg_glDeleteRenderbuffers
extern PFNGLBLITFRAMEBUFFERPROC sg_glBlitFramebuffer;
#define glBlitFramebuffer sg_glBlitFramebuffer
extern PFNGLBLENDFUNCSEPARATEPROC sg_glBlendFuncSeparate;
#define glBlendFuncSeparate sg_glBlendFuncSeparate
extern PFNGLBINDATTRIBLOCATIONPROC sg_glBindAttribLocation;
#define glBindAttribLocation sg_glBindAttribLocation
extern PFNGLGENVERTEXARRAYSPROC sg_glGenVertexArrays;
#define glGenVertexArrays sg_glGenVertexArrays
extern PFNGLBINDVERTEXARRAYPROC sg_glBindVertexArray;
#define glBindVertexArray sg_glBindVertexArray
extern PFNGLGENBUFFERSPROC sg_glGenBuffers;
#define glGenBuffers sg_glGenBuffers
extern PFNGLBINDBUFFERPROC sg_glBindBuffer;
#define glBindBuffer sg_glBindBuffer
extern PFNGLBUFFERDATAPROC sg_glBufferData;
#define glBufferData sg_glBufferData
extern PFNGLDELETEBUFFERSPROC sg_glDeleteBuffers;
#define glDeleteBuffers sg_glDeleteBuffers
extern PFNGLDELETEVERTEXARRAYSPROC sg_glDeleteVertexArrays;
#define glDeleteVertexArrays sg_glDeleteVertexArrays
extern PFNGLCREATESHADERPROC sg_glCreateShader;
#define glCreateShader sg_glCreateShader
extern PFNGLSHADERSOURCEPROC sg_glShaderSource;
#define glShaderSource sg_glShaderSource
extern PFNGLCOMPILESHADERPROC sg_glCompileShader;
#define glCompileShader sg_glCompileShader
extern PFNGLGETSHADERIVPROC sg_glGetShaderiv;
#define glGetShaderiv sg_glGetShaderiv
extern PFNGLGETSHADERINFOLOGPROC sg_glGetShaderInfoLog;
#define glGetShaderInfoLog sg_glGetShaderInfoLog
extern PFNGLCREATEPROGRAMPROC sg_glCreateProgram;
#define glCreateProgram sg_glCreateProgram
extern PFNGLATTACHSHADERPROC sg_glAttachShader;
#define glAttachShader sg_glAttachShader
extern PFNGLLINKPROGRAMPROC sg_glLinkProgram;
#define glLinkProgram sg_glLinkProgram
extern PFNGLGETPROGRAMIVPROC sg_glGetProgramiv;
#define glGetProgramiv sg_glGetProgramiv
extern PFNGLGETPROGRAMINFOLOGPROC sg_glGetProgramInfoLog;
#define glGetProgramInfoLog sg_glGetProgramInfoLog
extern PFNGLDELETESHADERPROC sg_glDeleteShader;
#define glDeleteShader sg_glDeleteShader
extern PFNGLDELETEPROGRAMPROC sg_glDeleteProgram;
#define glDeleteProgram sg_glDeleteProgram
extern PFNGLUSEPROGRAMPROC sg_glUseProgram;
#define glUseProgram sg_glUseProgram
extern PFNGLGETUNIFORMLOCATIONPROC sg_glGetUniformLocation;
#define glGetUniformLocation sg_glGetUniformLocation
extern PFNGLUNIFORMMATRIX4FVPROC sg_glUniformMatrix4fv;
#define glUniformMatrix4fv sg_glUniformMatrix4fv
extern PFNGLUNIFORM3FVPROC sg_glUniform3fv;
#define glUniform3fv sg_glUniform3fv
extern PFNGLUNIFORM4FVPROC sg_glUniform4fv;
#define glUniform4fv sg_glUniform4fv
extern PFNGLUNIFORM1FPROC sg_glUniform1f;
#define glUniform1f sg_glUniform1f
extern PFNGLUNIFORM1IPROC sg_glUniform1i;
#define glUniform1i sg_glUniform1i
extern PFNGLVERTEXATTRIBPOINTERPROC sg_glVertexAttribPointer;
#define glVertexAttribPointer sg_glVertexAttribPointer
extern PFNGLENABLEVERTEXATTRIBARRAYPROC sg_glEnableVertexAttribArray;
#define glEnableVertexAttribArray sg_glEnableVertexAttribArray
extern PFNGLGENFRAMEBUFFERSPROC sg_glGenFramebuffers;
#define glGenFramebuffers sg_glGenFramebuffers
extern PFNGLBINDFRAMEBUFFERPROC sg_glBindFramebuffer;
#define glBindFramebuffer sg_glBindFramebuffer
extern PFNGLFRAMEBUFFERTEXTURE2DPROC sg_glFramebufferTexture2D;
#define glFramebufferTexture2D sg_glFramebufferTexture2D
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC sg_glCheckFramebufferStatus;
#define glCheckFramebufferStatus sg_glCheckFramebufferStatus
extern PFNGLDELETEFRAMEBUFFERSPROC sg_glDeleteFramebuffers;
#define glDeleteFramebuffers sg_glDeleteFramebuffers
extern PFNGLDRAWBUFFERSPROC sg_glDrawBuffers;
#define glDrawBuffers sg_glDrawBuffers
extern PFNGLACTIVETEXTUREPROC sg_glActiveTexture;
#define glActiveTexture sg_glActiveTexture
extern PFNGLGENERATEMIPMAPPROC sg_glGenerateMipmap;
#define glGenerateMipmap sg_glGenerateMipmap
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#else
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
void *sg_platform_create(void);
int sg_platform_make_current(void *platform);
void sg_platform_free(void *platform);
const char *sg_platform_error(void);
#ifdef __cplusplus
}
#endif
#endif
