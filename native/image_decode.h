#ifndef BUOY_SCENE3D_IMAGE_DECODE_H
#define BUOY_SCENE3D_IMAGE_DECODE_H
#include <stdint.h>
#if defined(_WIN32)
#define SGIMAGE_API __declspec(dllexport)
#else
#define SGIMAGE_API __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif
/* mime: 1 PNG, 2 JPEG. Returns owned immutable straight-RGBA image or NULL.
 * Encoded input <=16MiB; each dimension <=4096; pixels <=max_pixels<=16Mi.
 * The input is borrowed only for this call; no filesystem access. */
SGIMAGE_API void *sgimage_decode(const void *data, int64_t length, int64_t mime, int64_t max_pixels);
SGIMAGE_API const void *sgimage_pixels(const void *image);
SGIMAGE_API int64_t sgimage_width(const void *image);
SGIMAGE_API int64_t sgimage_height(const void *image);
SGIMAGE_API void sgimage_free(void *image);
#ifdef __cplusplus
}
#endif
#endif
