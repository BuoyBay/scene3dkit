#include "platform.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
#define SG_THREAD_LOCAL __declspec(thread)
#else
#define SG_THREAD_LOCAL _Thread_local
#endif
static SG_THREAD_LOCAL char sg_last_error[256];
const char *sg_platform_error(void) { return sg_last_error; }

#if defined(_WIN32)
PFNGLGENRENDERBUFFERSPROC sg_glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC sg_glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC sg_glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERRENDERBUFFERPROC sg_glFramebufferRenderbuffer;
PFNGLDELETERENDERBUFFERSPROC sg_glDeleteRenderbuffers;
PFNGLBLITFRAMEBUFFERPROC sg_glBlitFramebuffer;
PFNGLBLENDFUNCSEPARATEPROC sg_glBlendFuncSeparate;
PFNGLBINDATTRIBLOCATIONPROC sg_glBindAttribLocation;
PFNGLGENVERTEXARRAYSPROC sg_glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC sg_glBindVertexArray;
PFNGLGENBUFFERSPROC sg_glGenBuffers;
PFNGLBINDBUFFERPROC sg_glBindBuffer;
PFNGLBUFFERDATAPROC sg_glBufferData;
PFNGLDELETEBUFFERSPROC sg_glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC sg_glDeleteVertexArrays;
PFNGLCREATESHADERPROC sg_glCreateShader;
PFNGLSHADERSOURCEPROC sg_glShaderSource;
PFNGLCOMPILESHADERPROC sg_glCompileShader;
PFNGLGETSHADERIVPROC sg_glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC sg_glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC sg_glCreateProgram;
PFNGLATTACHSHADERPROC sg_glAttachShader;
PFNGLLINKPROGRAMPROC sg_glLinkProgram;
PFNGLGETPROGRAMIVPROC sg_glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC sg_glGetProgramInfoLog;
PFNGLDELETESHADERPROC sg_glDeleteShader;
PFNGLDELETEPROGRAMPROC sg_glDeleteProgram;
PFNGLUSEPROGRAMPROC sg_glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC sg_glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC sg_glUniformMatrix4fv;
PFNGLUNIFORM3FVPROC sg_glUniform3fv;
PFNGLUNIFORM4FVPROC sg_glUniform4fv;
PFNGLUNIFORM1FPROC sg_glUniform1f;
PFNGLUNIFORM1IPROC sg_glUniform1i;
PFNGLVERTEXATTRIBPOINTERPROC sg_glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC sg_glEnableVertexAttribArray;
PFNGLGENFRAMEBUFFERSPROC sg_glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC sg_glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC sg_glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC sg_glCheckFramebufferStatus;
PFNGLDELETEFRAMEBUFFERSPROC sg_glDeleteFramebuffers;
PFNGLDRAWBUFFERSPROC sg_glDrawBuffers;
PFNGLACTIVETEXTUREPROC sg_glActiveTexture;
PFNGLGENERATEMIPMAPPROC sg_glGenerateMipmap;
typedef HGLRC (WINAPI *sg_create_context_attribs)(HDC,HGLRC,const int*);
typedef struct { HWND window; HDC dc; HGLRC context; DWORD owner; } sg_platform;
static PROC sg_win_proc(const char *name) {
    PROC p = wglGetProcAddress(name);
    if (p == NULL || p == (PROC)(intptr_t)1 || p == (PROC)(intptr_t)2 || p == (PROC)(intptr_t)3 || p == (PROC)(intptr_t)-1) {
        HMODULE gl = GetModuleHandleA("opengl32.dll");
        p = gl ? GetProcAddress(gl,name) : NULL;
    }
    return p;
}
static int sg_load_gl(void) {
    { PROC proc=sg_win_proc("glGenRenderbuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glGenRenderbuffers),"GL function pointer size"); memcpy(&sg_glGenRenderbuffers,&proc,sizeof(proc)); }
    if (!sg_glGenRenderbuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGenRenderbuffers"); return 0; }
    { PROC proc=sg_win_proc("glBindRenderbuffer"); _Static_assert(sizeof(proc)==sizeof(sg_glBindRenderbuffer),"GL function pointer size"); memcpy(&sg_glBindRenderbuffer,&proc,sizeof(proc)); }
    if (!sg_glBindRenderbuffer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBindRenderbuffer"); return 0; }
    { PROC proc=sg_win_proc("glRenderbufferStorageMultisample"); _Static_assert(sizeof(proc)==sizeof(sg_glRenderbufferStorageMultisample),"GL function pointer size"); memcpy(&sg_glRenderbufferStorageMultisample,&proc,sizeof(proc)); }
    if (!sg_glRenderbufferStorageMultisample) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glRenderbufferStorageMultisample"); return 0; }
    { PROC proc=sg_win_proc("glFramebufferRenderbuffer"); _Static_assert(sizeof(proc)==sizeof(sg_glFramebufferRenderbuffer),"GL function pointer size"); memcpy(&sg_glFramebufferRenderbuffer,&proc,sizeof(proc)); }
    if (!sg_glFramebufferRenderbuffer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glFramebufferRenderbuffer"); return 0; }
    { PROC proc=sg_win_proc("glDeleteRenderbuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteRenderbuffers),"GL function pointer size"); memcpy(&sg_glDeleteRenderbuffers,&proc,sizeof(proc)); }
    if (!sg_glDeleteRenderbuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteRenderbuffers"); return 0; }
    { PROC proc=sg_win_proc("glBlitFramebuffer"); _Static_assert(sizeof(proc)==sizeof(sg_glBlitFramebuffer),"GL function pointer size"); memcpy(&sg_glBlitFramebuffer,&proc,sizeof(proc)); }
    if (!sg_glBlitFramebuffer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBlitFramebuffer"); return 0; }
    { PROC proc=sg_win_proc("glBlendFuncSeparate"); _Static_assert(sizeof(proc)==sizeof(sg_glBlendFuncSeparate),"GL function pointer size"); memcpy(&sg_glBlendFuncSeparate,&proc,sizeof(proc)); }
    if (!sg_glBlendFuncSeparate) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBlendFuncSeparate"); return 0; }
    { PROC proc=sg_win_proc("glBindAttribLocation"); _Static_assert(sizeof(proc)==sizeof(sg_glBindAttribLocation),"GL function pointer size"); memcpy(&sg_glBindAttribLocation,&proc,sizeof(proc)); }
    if (!sg_glBindAttribLocation) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBindAttribLocation"); return 0; }
    { PROC proc=sg_win_proc("glGenVertexArrays"); _Static_assert(sizeof(proc)==sizeof(sg_glGenVertexArrays),"GL function pointer size"); memcpy(&sg_glGenVertexArrays,&proc,sizeof(proc)); } if (!sg_glGenVertexArrays) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGenVertexArrays"); return 0; }
    { PROC proc=sg_win_proc("glBindVertexArray"); _Static_assert(sizeof(proc)==sizeof(sg_glBindVertexArray),"GL function pointer size"); memcpy(&sg_glBindVertexArray,&proc,sizeof(proc)); } if (!sg_glBindVertexArray) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBindVertexArray"); return 0; }
    { PROC proc=sg_win_proc("glGenBuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glGenBuffers),"GL function pointer size"); memcpy(&sg_glGenBuffers,&proc,sizeof(proc)); } if (!sg_glGenBuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGenBuffers"); return 0; }
    { PROC proc=sg_win_proc("glBindBuffer"); _Static_assert(sizeof(proc)==sizeof(sg_glBindBuffer),"GL function pointer size"); memcpy(&sg_glBindBuffer,&proc,sizeof(proc)); } if (!sg_glBindBuffer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBindBuffer"); return 0; }
    { PROC proc=sg_win_proc("glBufferData"); _Static_assert(sizeof(proc)==sizeof(sg_glBufferData),"GL function pointer size"); memcpy(&sg_glBufferData,&proc,sizeof(proc)); } if (!sg_glBufferData) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBufferData"); return 0; }
    { PROC proc=sg_win_proc("glDeleteBuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteBuffers),"GL function pointer size"); memcpy(&sg_glDeleteBuffers,&proc,sizeof(proc)); } if (!sg_glDeleteBuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteBuffers"); return 0; }
    { PROC proc=sg_win_proc("glDeleteVertexArrays"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteVertexArrays),"GL function pointer size"); memcpy(&sg_glDeleteVertexArrays,&proc,sizeof(proc)); } if (!sg_glDeleteVertexArrays) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteVertexArrays"); return 0; }
    { PROC proc=sg_win_proc("glCreateShader"); _Static_assert(sizeof(proc)==sizeof(sg_glCreateShader),"GL function pointer size"); memcpy(&sg_glCreateShader,&proc,sizeof(proc)); } if (!sg_glCreateShader) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glCreateShader"); return 0; }
    { PROC proc=sg_win_proc("glShaderSource"); _Static_assert(sizeof(proc)==sizeof(sg_glShaderSource),"GL function pointer size"); memcpy(&sg_glShaderSource,&proc,sizeof(proc)); } if (!sg_glShaderSource) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glShaderSource"); return 0; }
    { PROC proc=sg_win_proc("glCompileShader"); _Static_assert(sizeof(proc)==sizeof(sg_glCompileShader),"GL function pointer size"); memcpy(&sg_glCompileShader,&proc,sizeof(proc)); } if (!sg_glCompileShader) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glCompileShader"); return 0; }
    { PROC proc=sg_win_proc("glGetShaderiv"); _Static_assert(sizeof(proc)==sizeof(sg_glGetShaderiv),"GL function pointer size"); memcpy(&sg_glGetShaderiv,&proc,sizeof(proc)); } if (!sg_glGetShaderiv) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGetShaderiv"); return 0; }
    { PROC proc=sg_win_proc("glGetShaderInfoLog"); _Static_assert(sizeof(proc)==sizeof(sg_glGetShaderInfoLog),"GL function pointer size"); memcpy(&sg_glGetShaderInfoLog,&proc,sizeof(proc)); } if (!sg_glGetShaderInfoLog) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGetShaderInfoLog"); return 0; }
    { PROC proc=sg_win_proc("glCreateProgram"); _Static_assert(sizeof(proc)==sizeof(sg_glCreateProgram),"GL function pointer size"); memcpy(&sg_glCreateProgram,&proc,sizeof(proc)); } if (!sg_glCreateProgram) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glCreateProgram"); return 0; }
    { PROC proc=sg_win_proc("glAttachShader"); _Static_assert(sizeof(proc)==sizeof(sg_glAttachShader),"GL function pointer size"); memcpy(&sg_glAttachShader,&proc,sizeof(proc)); } if (!sg_glAttachShader) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glAttachShader"); return 0; }
    { PROC proc=sg_win_proc("glLinkProgram"); _Static_assert(sizeof(proc)==sizeof(sg_glLinkProgram),"GL function pointer size"); memcpy(&sg_glLinkProgram,&proc,sizeof(proc)); } if (!sg_glLinkProgram) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glLinkProgram"); return 0; }
    { PROC proc=sg_win_proc("glGetProgramiv"); _Static_assert(sizeof(proc)==sizeof(sg_glGetProgramiv),"GL function pointer size"); memcpy(&sg_glGetProgramiv,&proc,sizeof(proc)); } if (!sg_glGetProgramiv) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGetProgramiv"); return 0; }
    { PROC proc=sg_win_proc("glGetProgramInfoLog"); _Static_assert(sizeof(proc)==sizeof(sg_glGetProgramInfoLog),"GL function pointer size"); memcpy(&sg_glGetProgramInfoLog,&proc,sizeof(proc)); } if (!sg_glGetProgramInfoLog) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGetProgramInfoLog"); return 0; }
    { PROC proc=sg_win_proc("glDeleteShader"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteShader),"GL function pointer size"); memcpy(&sg_glDeleteShader,&proc,sizeof(proc)); } if (!sg_glDeleteShader) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteShader"); return 0; }
    { PROC proc=sg_win_proc("glDeleteProgram"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteProgram),"GL function pointer size"); memcpy(&sg_glDeleteProgram,&proc,sizeof(proc)); } if (!sg_glDeleteProgram) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteProgram"); return 0; }
    { PROC proc=sg_win_proc("glUseProgram"); _Static_assert(sizeof(proc)==sizeof(sg_glUseProgram),"GL function pointer size"); memcpy(&sg_glUseProgram,&proc,sizeof(proc)); } if (!sg_glUseProgram) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUseProgram"); return 0; }
    { PROC proc=sg_win_proc("glGetUniformLocation"); _Static_assert(sizeof(proc)==sizeof(sg_glGetUniformLocation),"GL function pointer size"); memcpy(&sg_glGetUniformLocation,&proc,sizeof(proc)); } if (!sg_glGetUniformLocation) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGetUniformLocation"); return 0; }
    { PROC proc=sg_win_proc("glUniformMatrix4fv"); _Static_assert(sizeof(proc)==sizeof(sg_glUniformMatrix4fv),"GL function pointer size"); memcpy(&sg_glUniformMatrix4fv,&proc,sizeof(proc)); } if (!sg_glUniformMatrix4fv) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUniformMatrix4fv"); return 0; }
    { PROC proc=sg_win_proc("glUniform3fv"); _Static_assert(sizeof(proc)==sizeof(sg_glUniform3fv),"GL function pointer size"); memcpy(&sg_glUniform3fv,&proc,sizeof(proc)); } if (!sg_glUniform3fv) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUniform3fv"); return 0; }
    { PROC proc=sg_win_proc("glUniform4fv"); _Static_assert(sizeof(proc)==sizeof(sg_glUniform4fv),"GL function pointer size"); memcpy(&sg_glUniform4fv,&proc,sizeof(proc)); } if (!sg_glUniform4fv) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUniform4fv"); return 0; }
    { PROC proc=sg_win_proc("glUniform1f"); _Static_assert(sizeof(proc)==sizeof(sg_glUniform1f),"GL function pointer size"); memcpy(&sg_glUniform1f,&proc,sizeof(proc)); } if (!sg_glUniform1f) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUniform1f"); return 0; }
    { PROC proc=sg_win_proc("glUniform1i"); _Static_assert(sizeof(proc)==sizeof(sg_glUniform1i),"GL function pointer size"); memcpy(&sg_glUniform1i,&proc,sizeof(proc)); } if (!sg_glUniform1i) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glUniform1i"); return 0; }
    { PROC proc=sg_win_proc("glVertexAttribPointer"); _Static_assert(sizeof(proc)==sizeof(sg_glVertexAttribPointer),"GL function pointer size"); memcpy(&sg_glVertexAttribPointer,&proc,sizeof(proc)); } if (!sg_glVertexAttribPointer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glVertexAttribPointer"); return 0; }
    { PROC proc=sg_win_proc("glEnableVertexAttribArray"); _Static_assert(sizeof(proc)==sizeof(sg_glEnableVertexAttribArray),"GL function pointer size"); memcpy(&sg_glEnableVertexAttribArray,&proc,sizeof(proc)); } if (!sg_glEnableVertexAttribArray) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glEnableVertexAttribArray"); return 0; }
    { PROC proc=sg_win_proc("glGenFramebuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glGenFramebuffers),"GL function pointer size"); memcpy(&sg_glGenFramebuffers,&proc,sizeof(proc)); } if (!sg_glGenFramebuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGenFramebuffers"); return 0; }
    { PROC proc=sg_win_proc("glBindFramebuffer"); _Static_assert(sizeof(proc)==sizeof(sg_glBindFramebuffer),"GL function pointer size"); memcpy(&sg_glBindFramebuffer,&proc,sizeof(proc)); } if (!sg_glBindFramebuffer) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glBindFramebuffer"); return 0; }
    { PROC proc=sg_win_proc("glFramebufferTexture2D"); _Static_assert(sizeof(proc)==sizeof(sg_glFramebufferTexture2D),"GL function pointer size"); memcpy(&sg_glFramebufferTexture2D,&proc,sizeof(proc)); } if (!sg_glFramebufferTexture2D) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glFramebufferTexture2D"); return 0; }
    { PROC proc=sg_win_proc("glCheckFramebufferStatus"); _Static_assert(sizeof(proc)==sizeof(sg_glCheckFramebufferStatus),"GL function pointer size"); memcpy(&sg_glCheckFramebufferStatus,&proc,sizeof(proc)); } if (!sg_glCheckFramebufferStatus) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glCheckFramebufferStatus"); return 0; }
    { PROC proc=sg_win_proc("glDeleteFramebuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glDeleteFramebuffers),"GL function pointer size"); memcpy(&sg_glDeleteFramebuffers,&proc,sizeof(proc)); } if (!sg_glDeleteFramebuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDeleteFramebuffers"); return 0; }
    { PROC proc=sg_win_proc("glDrawBuffers"); _Static_assert(sizeof(proc)==sizeof(sg_glDrawBuffers),"GL function pointer size"); memcpy(&sg_glDrawBuffers,&proc,sizeof(proc)); } if (!sg_glDrawBuffers) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glDrawBuffers"); return 0; }
    { PROC proc=sg_win_proc("glActiveTexture"); _Static_assert(sizeof(proc)==sizeof(sg_glActiveTexture),"GL function pointer size"); memcpy(&sg_glActiveTexture,&proc,sizeof(proc)); } if (!sg_glActiveTexture) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glActiveTexture"); return 0; }
    { PROC proc=sg_win_proc("glGenerateMipmap"); _Static_assert(sizeof(proc)==sizeof(sg_glGenerateMipmap),"GL function pointer size"); memcpy(&sg_glGenerateMipmap,&proc,sizeof(proc)); } if (!sg_glGenerateMipmap) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL entry point unavailable: glGenerateMipmap"); return 0; }
    return 1;
}
static LRESULT CALLBACK sg_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_APP + 73) {
        sg_platform_free((void *)lparam);
        return 0;
    }
    return DefWindowProcA(window, message, wparam, lparam);
}
void sg_platform_free(void *opaque) {
    sg_platform *p=(sg_platform*)opaque;
    if (!p) return;
    if (p->owner != GetCurrentThreadId()) {
        /* SendMessage executes the teardown on the window's owning thread.
         * Close must be called while that thread is servicing messages. */
        SendMessageA(p->window, WM_APP + 73, 0, (LPARAM)p);
        return;
    }
    if (p->context) {
        if (wglGetCurrentContext()==p->context) wglMakeCurrent(NULL,NULL);
        wglDeleteContext(p->context);
    }
    if(p->dc && p->window) ReleaseDC(p->window,p->dc);
    if(p->window) DestroyWindow(p->window);
    free(p);
}
int sg_platform_make_current(void *opaque) {
    sg_platform *p=(sg_platform*)opaque;
    if (!p || !p->context) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context is closed"); return 0; }
    if (p->owner != GetCurrentThreadId()) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context belongs to another thread"); return 0; }
    if (!wglMakeCurrent(p->dc,p->context)) { snprintf(sg_last_error,sizeof(sg_last_error),"wglMakeCurrent failed (%lu)",(unsigned long)GetLastError()); return 0; }
    return 1;
}
void *sg_platform_create(void) {
    sg_last_error[0]=0;
    sg_platform *p=(sg_platform*)calloc(1,sizeof(*p));
    if(!p) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU platform allocation failed"); return NULL; }
    p->owner=GetCurrentThreadId();
    HINSTANCE instance=GetModuleHandleA(NULL);
    WNDCLASSA wc={0}; wc.style=CS_OWNDC; wc.lpfnWndProc=sg_window_proc;
    wc.hInstance=instance; wc.lpszClassName="Scene3DKitOffscreenOpenGL";
    if (!RegisterClassA(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS) goto fail;
    p->window=CreateWindowExA(0,wc.lpszClassName,"",WS_POPUP,0,0,1,1,NULL,NULL,instance,NULL);
    if(!p->window) goto fail;
    p->dc=GetDC(p->window); if(!p->dc) goto fail;
    PIXELFORMATDESCRIPTOR pf={0}; pf.nSize=sizeof(pf); pf.nVersion=1;
    pf.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL; pf.iPixelType=PFD_TYPE_RGBA;
    pf.cColorBits=32; pf.cDepthBits=24; pf.cStencilBits=8; pf.iLayerType=PFD_MAIN_PLANE;
    int format=ChoosePixelFormat(p->dc,&pf);
    if(!format || !SetPixelFormat(p->dc,format,&pf)) goto fail;
    p->context=wglCreateContext(p->dc); if(!p->context || !wglMakeCurrent(p->dc,p->context)) goto fail;
    sg_create_context_attribs create=NULL;
    { PROC proc=sg_win_proc("wglCreateContextAttribsARB"); _Static_assert(sizeof(proc)==sizeof(create),"WGL function pointer size"); memcpy(&create,&proc,sizeof(proc)); }
    if(!create) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL 3.3 context creation unavailable"); goto fail; }
    const int attrs[]={0x2091,3,0x2092,3,0x9126,1,0};
    HGLRC core=create(p->dc,NULL,attrs);
    if(!core) { snprintf(sg_last_error,sizeof(sg_last_error),"OpenGL 3.3 core context unavailable"); goto fail; }
    wglMakeCurrent(NULL,NULL); wglDeleteContext(p->context); p->context=core;
    if(!wglMakeCurrent(p->dc,core)) goto fail;
    if(!sg_load_gl()) goto fail;
    return p;
fail:
    if(!sg_last_error[0]) snprintf(sg_last_error,sizeof(sg_last_error),"Windows offscreen OpenGL creation failed (%lu)",(unsigned long)GetLastError());
    sg_platform_free(p); return NULL;
}
#elif defined(__APPLE__)
#include <pthread.h>
typedef struct { CGLContextObj context; pthread_t owner; } sg_platform;
void sg_platform_free(void *opaque) {
    sg_platform *p=(sg_platform*)opaque; if(!p)return;
    if(p->context) { if(CGLGetCurrentContext()==p->context) CGLSetCurrentContext(NULL); CGLDestroyContext(p->context); }
    free(p);
}
int sg_platform_make_current(void *opaque) {
    sg_platform *p=(sg_platform*)opaque;
    if(!p || !p->context) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context is closed"); return 0; }
    if(!pthread_equal(p->owner,pthread_self())) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context belongs to another thread"); return 0; }
    CGLError e=CGLSetCurrentContext(p->context);
    if(e!=kCGLNoError) { snprintf(sg_last_error,sizeof(sg_last_error),"CGLSetCurrentContext: %s",CGLErrorString(e)); return 0; }
    return 1;
}
void *sg_platform_create(void) {
    sg_last_error[0]=0;
    CGLPixelFormatAttribute attrs[]={kCGLPFAOpenGLProfile,(CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,kCGLPFAColorSize,(CGLPixelFormatAttribute)32,kCGLPFADepthSize,(CGLPixelFormatAttribute)24,kCGLPFAAllowOfflineRenderers,(CGLPixelFormatAttribute)0};
    CGLPixelFormatObj format=NULL; GLint count=0;
    CGLError e=CGLChoosePixelFormat(attrs,&format,&count);
    if(e!=kCGLNoError || !format || !count) { snprintf(sg_last_error,sizeof(sg_last_error),"CGL OpenGL 3.2 pixel format unavailable: %s",CGLErrorString(e)); if(format)CGLDestroyPixelFormat(format); return NULL; }
    sg_platform *p=(sg_platform*)calloc(1,sizeof(*p));
    if(!p) { CGLDestroyPixelFormat(format); snprintf(sg_last_error,sizeof(sg_last_error),"GPU platform allocation failed"); return NULL; }
    p->owner=pthread_self(); e=CGLCreateContext(format,NULL,&p->context); CGLDestroyPixelFormat(format);
    if(e!=kCGLNoError) { snprintf(sg_last_error,sizeof(sg_last_error),"CGLCreateContext: %s",CGLErrorString(e)); sg_platform_free(p); return NULL; }
    if(!sg_platform_make_current(p)) { sg_platform_free(p); return NULL; }
    return p;
}
#else
#include <pthread.h>
typedef struct { EGLDisplay display; EGLSurface surface; EGLContext context; pthread_t owner; } sg_platform;
/* eglInitialize has no per-caller reference count. Keep shared display lifetime
 * explicit so destroying one renderer cannot terminate another renderer. */
static pthread_mutex_t sg_display_lock=PTHREAD_MUTEX_INITIALIZER;
static EGLDisplay sg_display=EGL_NO_DISPLAY;
static unsigned sg_display_users=0;
static EGLDisplay sg_acquire_display(void) {
    pthread_mutex_lock(&sg_display_lock);
    if(sg_display_users) { ++sg_display_users; EGLDisplay d=sg_display; pthread_mutex_unlock(&sg_display_lock); return d; }
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform=(PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    EGLDisplay display=EGL_NO_DISPLAY;
    if(get_platform) display=get_platform(0x31DD,EGL_DEFAULT_DISPLAY,NULL); /* EGL_PLATFORM_SURFACELESS_MESA */
    if(display==EGL_NO_DISPLAY || !eglInitialize(display,NULL,NULL)) {
        display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if(display==EGL_NO_DISPLAY || !eglInitialize(display,NULL,NULL)) { snprintf(sg_last_error,sizeof(sg_last_error),"EGL display initialization failed (0x%x)",eglGetError()); pthread_mutex_unlock(&sg_display_lock); return EGL_NO_DISPLAY; }
    }
    sg_display=display; sg_display_users=1; pthread_mutex_unlock(&sg_display_lock); return display;
}
static void sg_release_display(void) {
    pthread_mutex_lock(&sg_display_lock);
    if(sg_display_users && --sg_display_users==0) { eglTerminate(sg_display); sg_display=EGL_NO_DISPLAY; }
    pthread_mutex_unlock(&sg_display_lock);
}
void sg_platform_free(void *opaque) {
    sg_platform *p=(sg_platform*)opaque; if(!p)return;
    if(p->display!=EGL_NO_DISPLAY) {
        if(eglGetCurrentContext()==p->context) eglMakeCurrent(p->display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
        if(p->context!=EGL_NO_CONTEXT) eglDestroyContext(p->display,p->context);
        if(p->surface!=EGL_NO_SURFACE) eglDestroySurface(p->display,p->surface);
        sg_release_display();
    }
    free(p);
}
int sg_platform_make_current(void *opaque) {
    sg_platform *p=(sg_platform*)opaque;
    if(p && !pthread_equal(p->owner,pthread_self())) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context belongs to another thread"); return 0; }
    if(!p || p->context==EGL_NO_CONTEXT) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context is closed"); return 0; }
    if(!pthread_equal(p->owner,pthread_self())) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU context belongs to another thread"); return 0; }
    if(!eglMakeCurrent(p->display,p->surface,p->surface,p->context)) { snprintf(sg_last_error,sizeof(sg_last_error),"eglMakeCurrent failed (0x%x)",eglGetError()); return 0; }
    return 1;
}
void *sg_platform_create(void) {
    sg_last_error[0]=0;
    sg_platform *p=(sg_platform*)calloc(1,sizeof(*p));
    if(!p) { snprintf(sg_last_error,sizeof(sg_last_error),"GPU platform allocation failed"); return NULL; }
    p->owner=pthread_self(); p->display=sg_acquire_display(); p->surface=EGL_NO_SURFACE; p->context=EGL_NO_CONTEXT;
    if(p->display==EGL_NO_DISPLAY) { free(p); return NULL; }
    if(!eglBindAPI(EGL_OPENGL_ES_API)) goto fail;
    const EGLint attributes[]={EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,EGL_RENDERABLE_TYPE,0x0040,EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_ALPHA_SIZE,8,EGL_DEPTH_SIZE,24,EGL_NONE};
    EGLConfig config=NULL; EGLint count=0;
    if(!eglChooseConfig(p->display,attributes,&config,1,&count) || count<1) goto fail;
    const EGLint surface_attributes[]={EGL_WIDTH,1,EGL_HEIGHT,1,EGL_NONE};
    p->surface=eglCreatePbufferSurface(p->display,config,surface_attributes);
    if(p->surface==EGL_NO_SURFACE) goto fail;
    const EGLint context_attributes[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    p->context=eglCreateContext(p->display,config,EGL_NO_CONTEXT,context_attributes);
    if(p->context==EGL_NO_CONTEXT) goto fail;
    if(!sg_platform_make_current(p)) { sg_platform_free(p); return NULL; }
    return p;
fail:
    snprintf(sg_last_error,sizeof(sg_last_error),"EGL ES3 offscreen context unavailable (0x%x)",eglGetError());
    sg_platform_free(p); return NULL;
}
#endif
