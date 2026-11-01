#ifndef BUOY_SCENE3D_H
#define BUOY_SCENE3D_H
#include <stdint.h>
/* Native offscreen renderer. Every context call, including destruction, belongs
 * to the creating thread. Mesh/texture IDs are local to that context; zero is
 * failure/no texture. Error strings remain owned by the context (NULL retrieves
 * the creating thread's last creation error). Matrices are column-major.
 *
 * Vertex layouts are doubles: stride12 = position3 normal3 UV0two RGBA4;
 * stride14 adds UV1two before RGBA4. Inputs are copied before calls return.
 * 512 mesh slots (ID0 reserved), 256 texture slots (ID0 reserved), 4096 queued
 * draws, 2M aggregate vertices and 16M aggregate texture pixels per context.
 * Frame dimensions are1..4096, shadow map1024. Queued resources cannot be
 * freed.
 *
 * Scene color is RGBA16F linear HDR; final ACES/gamma tone mapping happens once
 * after all opaque, blend and additive draws.
 * alpha_mode0=opaque,1=mask,2=blend, 3=additive (SRC_ALPHA,ONE RGB with
 * coverage alpha union). Additive/blended draws do not write depth or cast
 * shadows. Unlit uses base+emissive radiance; lit uses PBR+emissive. Exposure
 * scales radiance before blending. HDR radiance is bounded to60000 per fragment
 * before half-float storage. begin uses surface-to-light direction and straight
 * RGBA clear color. render consumes the queue, retaining top-down straight
 * RGBA8 until next render/free. Display-space clear colors are
 * inverse-tone-mapped during HDR initialization to preserve the public
 * clear/readback contract, including partial alpha. Color textures are sRGB;
 * MR/normal/AO textures are linear. Environment upload is atomic: finite
 * little-endian RGBA16F cube mipmajor, diffuse16x16x6x1level (12288bytes) and
 * specular128x128x6x8levels (1048560bytes).
 *
 * Opaque/mask output is opaque; blended draws are sorted by object centroid
 * distance, with depth writes off. Intersecting transparency remains
 * approximate.
 */
#if defined(_WIN32)
#define SG3_API __declspec(dllexport)
#else
#define SG3_API __attribute__((visibility("default")))
#endif
#ifdef __cplusplus
extern "C" {
#endif
SG3_API void *sg3_create(int width, int height);
SG3_API void *sg3_create_ex(int width, int height, int requested_samples);
SG3_API int sg3_set_directional_enabled(void *ctx, int enabled);
SG3_API int sg3_samples(void *ctx);
SG3_API int sg3_set_bloom(void *ctx, double strength);
SG3_API int sg3_clear_lights(void *ctx);
SG3_API int sg3_point_light(void *ctx, double x, double y, double z, double r,
                            double g, double b, double intensity, double range);
SG3_API int sg3_point_light_at(void *ctx, int slot, double x, double y,
                               double z, double r, double g, double b,
                               double intensity, double range);
SG3_API int sg3_spot_light(void *ctx, double x, double y, double z, double dx,
                           double dy, double dz, double r, double g, double b,
                           double intensity, double range, double inner,
                           double outer);
SG3_API int sg3_point_light_at_ex(void *ctx, int slot, double x, double y,
                                  double z, double r, double g, double b,
                                  double intensity, double range,
                                  int cast_shadows);
SG3_API int sg3_spot_light_ex(void *ctx, double x, double y, double z,
                              double dx, double dy, double dz, double r,
                              double g, double b, double intensity,
                              double range, double inner, double outer,
                              int cast_shadows);
SG3_API const char *sg3_error(void *ctx);
SG3_API const char *sg3_renderer(void *ctx);
SG3_API const char *sg3_version(void *ctx);
SG3_API int sg3_is_software(void *ctx);
SG3_API int sg3_mesh(void *ctx, const double *vertices, int vertex_count,
                     int stride);
SG3_API int sg3_mesh_free(void *ctx, int mesh);
SG3_API int sg3_texture(void *ctx, const uint8_t *rgba, int width, int height);
SG3_API int sg3_texture_ex(void *ctx, const uint8_t *rgba, int width,
                           int height, int wrap_s, int wrap_t, int min_filter,
                           int mag_filter);
SG3_API int sg3_texture_free(void *ctx, int texture);
SG3_API int sg3_environment(void *ctx, const void *diffuse, int diffuse_bytes,
                            const void *specular, int specular_bytes);
SG3_API int sg3_begin(void *ctx, const double *view_projection,
                      const double *eye, const double *clear_rgba,
                      const double *light_view_projection,
                      const double *light_direction, double exposure);
SG3_API int sg3_submit(void *ctx, int mesh, const double *model,
                       const double *base_rgba, double metallic,
                       double roughness, int texture, int cast_shadow);
SG3_API int sg3_submit_pbr(void *ctx, int mesh, const double *model,
                           const double *base_rgba, double metallic,
                           double roughness, int base_texture, int mr_texture,
                           int normal_texture, int occlusion_texture,
                           int emissive_texture, const double *emissive,
                           int alpha_mode, double alpha_cutoff,
                           int double_sided, int cast_shadow);
SG3_API int sg3_submit_material(void *ctx, int mesh, const double *model,
                                const double *base_rgba, double metallic,
                                double roughness, int base_texture,
                                int mr_texture, int normal_texture,
                                int occlusion_texture, int emissive_texture,
                                const double *emissive, int alpha_mode,
                                double alpha_cutoff, int double_sided,
                                int cast_shadow, const double *uv_selectors,
                                double normal_scale, double ao_strength);
SG3_API int sg3_submit_material_ex(
    void *ctx, int mesh, const double *model, const double *base,
    double metallic, double roughness, int base_texture, int mr_texture,
    int normal_texture, int occlusion_texture, int emissive_texture,
    const double *emissive, int alpha_mode, double alpha_cutoff,
    int double_sided, int cast_shadow, const double *uv_selectors,
    double normal_scale, double ao_strength, int unlit);
SG3_API int sg3_render(void *ctx);
SG3_API const uint8_t *sg3_pixels(void *ctx);
SG3_API int sg3_finish(void *ctx, uint8_t *rgba, int capacity);
SG3_API void sg3_free(void *ctx);
#ifdef __cplusplus
}
#endif
#endif
