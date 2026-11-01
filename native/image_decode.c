#include "image_decode.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>
/* Budget every decoder allocation, including malformed compressed streams that
 * expand beyond their declared image dimensions. Tags distinguish outstanding
 * output buffers from a later decode on the same thread. */
#if defined(_MSC_VER)
#define SGIMAGE_TLS __declspec(thread)
#else
#define SGIMAGE_TLS _Thread_local
#endif
typedef union { struct { size_t size; uint64_t generation; const void *owner; } info; max_align_t alignment; } sgimage_allocation;
static SGIMAGE_TLS size_t sgimage_used;
static SGIMAGE_TLS size_t sgimage_limit=16777216;
static SGIMAGE_TLS uint64_t sgimage_generation;
static void *sgimage_malloc(size_t size) {
    if (size>sgimage_limit || sgimage_used>sgimage_limit-size || size>SIZE_MAX-sizeof(sgimage_allocation)) return NULL;
    sgimage_allocation *p=(sgimage_allocation*)malloc(sizeof(*p)+size);
    if (!p) return NULL;
    p->info.size=size; p->info.generation=sgimage_generation; p->info.owner=&sgimage_used; sgimage_used+=size; return p+1;
}
static void sgimage_release(void *memory) {
    if (!memory) return;
    sgimage_allocation *p=((sgimage_allocation*)memory)-1;
    if (p->info.owner==&sgimage_used && p->info.generation==sgimage_generation && p->info.size<=sgimage_used) sgimage_used-=p->info.size;
    free(p);
}
static void *sgimage_realloc(void *memory,size_t size) {
    if (!memory) return sgimage_malloc(size);
    sgimage_allocation *p=((sgimage_allocation*)memory)-1;
    size_t old=(p->info.owner==&sgimage_used && p->info.generation==sgimage_generation)?p->info.size:0;
    if (sgimage_used<old || size>sgimage_limit || sgimage_used-old>sgimage_limit-size || size>SIZE_MAX-sizeof(*p)) return NULL;
    p=(sgimage_allocation*)realloc(p,sizeof(*p)+size); if (!p) return NULL;
    sgimage_used=sgimage_used-old+size; p->info.size=size; p->info.generation=sgimage_generation; p->info.owner=&sgimage_used; return p+1;
}
#define STBI_MALLOC(size) sgimage_malloc(size)
#define STBI_REALLOC(memory,size) sgimage_realloc(memory,size)
#define STBI_FREE(memory) sgimage_release(memory)
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_MAX_DIMENSIONS 4096
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

typedef struct { unsigned char *pixels; int width, height; } sgimage;
static uint32_t sgimage_be32(const unsigned char *p) {
    return (uint32_t)p[0]<<24 | (uint32_t)p[1]<<16 | (uint32_t)p[2]<<8 | p[3];
}
static uint32_t sgimage_crc32(const unsigned char *p, size_t n) {
    uint32_t c=0xffffffffu;
    while (n--) { c^=*p++; for (int i=0;i<8;i++) c=(c>>1)^((0u-(c&1u))&0xedb88320u); }
    return ~c;
}
/* Strict PNG framing and per-chunk CRC before the decoder allocates pixels. */
static size_t sgimage_png_valid(const unsigned char *p, size_t n) {
    static const unsigned char sig[8]={137,80,78,71,13,10,26,10};
    if (n<33 || memcmp(p,sig,8)) return 0;
    size_t at=8; int first=1;
    while (at<=n && n-at>=12) {
        uint32_t size=sgimage_be32(p+at);
        if ((uint64_t)size+12u>n-at) return 0;
        const unsigned char *type=p+at+4;
        if (first && (size!=13 || memcmp(type,"IHDR",4))) return 0;
        if (sgimage_crc32(type,(size_t)size+4)!=sgimage_be32(p+at+8+size)) return 0;
        first=0; at+=(size_t)size+12;
        if (!memcmp(type,"IEND",4)) return size==0 ? at : 0;
    }
    return 0;
}
SGIMAGE_API void *sgimage_decode(const void *input, int64_t length, int64_t mime, int64_t max_pixels) {
    const unsigned char *data=(const unsigned char*)input;
    if (!data || length<4 || length>16777216 || max_pixels<1 || max_pixels>16777216) return NULL;
    /* GLB buffer views may include zero alignment padding. Bound decoding to
     * the encoded prefix while retaining all framing and CRC checks. */
    if (mime==2) while (length>4 && data[length-1]==0) length--;
    if (mime==1) {
        size_t prefix=sgimage_png_valid(data,(size_t)length);
        if (!prefix) return NULL;
        length=(int64_t)prefix;
    } else if (mime==2) {
        if (data[0]!=0xff || data[1]!=0xd8 || data[length-2]!=0xff || data[length-1]!=0xd9) return NULL;
    } else return NULL;
    sgimage_generation++; if (!sgimage_generation) sgimage_generation++;
    sgimage_used=0; sgimage_limit=16777216;
    int w=0,h=0,channels=0;
    if (!stbi_info_from_memory(data,(int)length,&w,&h,&channels)) return NULL;
    if (w<1 || h<1 || w>4096 || h>4096 || (int64_t)w*h>max_pixels) return NULL;
    sgimage_limit=(size_t)w*(size_t)h*24u+16777216u;
    unsigned char *pixels=stbi_load_from_memory(data,(int)length,&w,&h,&channels,4);
    if (!pixels) return NULL;
    if (w<1 || h<1 || w>4096 || h>4096 || (int64_t)w*h>max_pixels) { stbi_image_free(pixels); return NULL; }
    sgimage *out=(sgimage*)malloc(sizeof(*out));
    if (!out) { stbi_image_free(pixels); return NULL; }
    out->pixels=pixels; out->width=w; out->height=h; return out;
}
SGIMAGE_API const void *sgimage_pixels(const void *image) { return image?((const sgimage*)image)->pixels:NULL; }
SGIMAGE_API int64_t sgimage_width(const void *image) { return image?((const sgimage*)image)->width:0; }
SGIMAGE_API int64_t sgimage_height(const void *image) { return image?((const sgimage*)image)->height:0; }
SGIMAGE_API void sgimage_free(void *image) { if (image) { sgimage *p=(sgimage*)image; stbi_image_free(p->pixels); free(p); } }
