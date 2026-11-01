#include "platform.h"
#include "punctual_shadows.h"
#include "scene3d.h"
#include "shaders.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_MESH 512
#define MAX_TEXTURE 256
#define MAX_DRAW 4096
#define SHADOW_SIZE 1024
#define MAX_VERTICES 786432
#if defined(_MSC_VER)
#define SG_TLS __declspec(thread)
#else
#define SG_TLS _Thread_local
#endif
static SG_TLS char global_error[2048];
typedef struct {
  GLuint vao, vbo;
  int count;
  float center[3];
} Mesh;
typedef struct {
  int mesh;
  int mirrored;
  float model[16], base[4], metallic, roughness, emissive[3], cutoff, uv[5],
      normal_scale, ao_strength;
  int tex[5], alpha_mode, double_sided, cast_shadow, unlit;
} Draw;
typedef struct {
  void *platform;
  int width, height;
  char error[2048], renderer[512], version[256];
  GLuint program, shadow_program, fbo, color, depth, shadow_fbo, shadow_depth,
      environment[2], white, tone_program, output_fbo, output_color, tone_vao,
      msaa_fbo, msaa_color, msaa_depth, bloom_fbo[2], bloom_color[2],
      bloom_program;
  SG3Shadows punctual_shadows;
  int samples, bloom_width, bloom_height;
  int point_shadow_slot, spot_shadow_enabled, directional_enabled;
  float bloom_strength, point_pos_range[32], point_color_intensity[32],
      spot_pos_range[4], spot_color_intensity[4], spot_direction_inner[4],
      spot_outer;
  Mesh meshes[MAX_MESH];
  GLuint textures[MAX_TEXTURE];
  int texture_pixels[MAX_TEXTURE];
  int total_vertices, total_texture_pixels;
  Draw draws[MAX_DRAW];
  int draw_count, begun, rendered, software, has_environment;
  float vp[16], eye[3], light_vp[16], light[3], clear[4], exposure;
  uint8_t *pixels, *bottom;
} Context;
static int fail(Context *c, const char *message) {
  snprintf(c ? c->error : global_error, 2048, "%s", message);
  return 0;
}
static int active(Context *c) {
  if (!c)
    return fail(NULL, "GPU context is null");
  if (!sg_platform_make_current(c->platform))
    return fail(c, sg_platform_error());
  return 1;
}
static int finite_array(const double *p, int n, double limit) {
  if (!p)
    return 0;
  for (int i = 0; i < n; i++)
    if (!isfinite(p[i]) || fabs(p[i]) > limit)
      return 0;
  return 1;
}
static void copy_float(float *to, const double *from, int n) {
  for (int i = 0; i < n; i++)
    to[i] = (float)from[i];
}
static GLuint shader(Context *c, GLenum type, const char *source) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &source, NULL);
  glCompileShader(s);
  GLint valid;
  glGetShaderiv(s, GL_COMPILE_STATUS, &valid);
  if (!valid) {
    glGetShaderInfoLog(s, sizeof(c->error), NULL, c->error);
    glDeleteShader(s);
    return 0;
  }
  return s;
}
static GLuint program(Context *c, const char *vertex, const char *fragment) {
  GLuint v = shader(c, GL_VERTEX_SHADER, vertex);
  if (!v)
    return 0;
  GLuint f = shader(c, GL_FRAGMENT_SHADER, fragment);
  if (!f) {
    glDeleteShader(v);
    return 0;
  }
  GLuint p = glCreateProgram();
  glAttachShader(p, v);
  glAttachShader(p, f);
  glBindAttribLocation(p, 0, "position");
  glBindAttribLocation(p, 1, "normal");
  glBindAttribLocation(p, 2, "uv");
  glBindAttribLocation(p, 3, "color");
  glBindAttribLocation(p, 4, "uv1");
  glLinkProgram(p);
  glDeleteShader(v);
  glDeleteShader(f);
  GLint valid;
  glGetProgramiv(p, GL_LINK_STATUS, &valid);
  if (!valid) {
    glGetProgramInfoLog(p, sizeof(c->error), NULL, c->error);
    glDeleteProgram(p);
    return 0;
  }
  return p;
}
static void tex_params(GLenum target) {
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
static int contains_ignore_case(const char *haystack, const char *needle) {
  size_t n = strlen(needle);
  for (; *haystack; haystack++) {
    size_t i = 0;
    while (i < n) {
      char a = haystack[i], b = needle[i];
      if (!a)
        break;
      if (a >= 'A' && a <= 'Z')
        a = (char)(a + 32);
      if (a != b)
        break;
      i++;
    }
    if (i == n)
      return 1;
  }
  return 0;
}
SG3_API void *sg3_create_ex(int width, int height, int requested_samples) {
  global_error[0] = 0;
  if (requested_samples != 0 && requested_samples != 1 &&
      requested_samples != 4) {
    fail(NULL, "Requested GPU samples must be1 or4");
    return NULL;
  }
  if (width < 1 || height < 1 || width > 4096 || height > 4096) {
    fail(NULL, "GPU dimensions must be 1..4096");
    return NULL;
  }
  Context *c = calloc(1, sizeof(*c));
  if (!c) {
    fail(NULL, "GPU allocation failed");
    return NULL;
  }
  c->directional_enabled = 1;
  sg3_shadow_init(&c->punctual_shadows);
  c->point_shadow_slot = -1;
  c->width = width;
  c->height = height;
  c->platform = sg_platform_create();
  if (!c->platform) {
    snprintf(global_error, sizeof(global_error), "%s", sg_platform_error());
    free(c);
    return NULL;
  }
  if (!active(c))
    goto bad;
  c->pixels = malloc((size_t)width * height * 4);
  c->bottom = malloc((size_t)width * height * 4);
  if (!c->pixels || !c->bottom) {
    fail(c, "Pixel allocation failed");
    goto bad;
  }
  const char *vendor = (const char *)glGetString(GL_VENDOR);
  const char *renderer = (const char *)glGetString(GL_RENDERER);
  const char *version = (const char *)glGetString(GL_VERSION);
  snprintf(c->renderer, sizeof(c->renderer), "%s / %s", vendor ? vendor : "", renderer ? renderer : "");
  snprintf(c->version, sizeof(c->version), "%s", version ? version : "");
  c->software = contains_ignore_case(c->renderer, "llvmpipe") ||
                contains_ignore_case(c->renderer, "softpipe") ||
                contains_ignore_case(c->renderer, "software") ||
                contains_ignore_case(c->renderer, "swiftshader") ||
                contains_ignore_case(c->renderer, "gdi generic");
  c->program = program(c, sg3_vertex_shader, sg3_fragment_shader);
  if (!c->program)
    goto bad;
  c->shadow_program =
      program(c, sg3_shadow_vertex_shader, sg3_shadow_fragment_shader);
  if (!c->shadow_program)
    goto bad;
  c->tone_program =
      program(c, sg3_tone_vertex_shader, sg3_tone_fragment_shader);
  if (!c->tone_program)
    goto bad;
  glGenVertexArrays(1, &c->tone_vao);
  glGenFramebuffers(1, &c->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, c->fbo);
  glGenTextures(1, &c->color);
  glBindTexture(GL_TEXTURE_2D, c->color);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_HALF_FLOAT, NULL);
  tex_params(GL_TEXTURE_2D);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         c->color, 0);
  glGenTextures(1, &c->depth);
  glBindTexture(GL_TEXTURE_2D, c->depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
               GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  tex_params(GL_TEXTURE_2D);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         c->depth, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fail(c, "Color/depth framebuffer unavailable");
    goto bad;
  }
  glGenFramebuffers(1, &c->output_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, c->output_fbo);
  glGenTextures(1, &c->output_color);
  glBindTexture(GL_TEXTURE_2D, c->output_color);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  tex_params(GL_TEXTURE_2D);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         c->output_color, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fail(c, "Tone-map output framebuffer unavailable");
    goto bad;
  }
  c->samples = 1;
  GLint max_samples = 1;
  glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
  if (requested_samples == 4 && max_samples >= 4) {
    glGenFramebuffers(1, &c->msaa_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, c->msaa_fbo);
    glGenRenderbuffers(1, &c->msaa_color);
    glBindRenderbuffer(GL_RENDERBUFFER, c->msaa_color);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, width,
                                     height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, c->msaa_color);
    glGenRenderbuffers(1, &c->msaa_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, c->msaa_depth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24,
                                     width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, c->msaa_depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE &&
        glGetError() == GL_NO_ERROR)
      c->samples = 4;
    else {
      while (glGetError() != GL_NO_ERROR) {
      }
      glDeleteFramebuffers(1, &c->msaa_fbo);
      glDeleteRenderbuffers(1, &c->msaa_color);
      glDeleteRenderbuffers(1, &c->msaa_depth);
      c->msaa_fbo = c->msaa_color = c->msaa_depth = 0;
    }
  }
  c->bloom_width = width > 1 ? width / 2 : 1;
  c->bloom_height = height > 1 ? height / 2 : 1;
  c->bloom_program =
      program(c, sg3_tone_vertex_shader, sg3_bloom_fragment_shader);
  if (!c->bloom_program)
    goto bad;
  glGenFramebuffers(2, c->bloom_fbo);
  glGenTextures(2, c->bloom_color);
  for (int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, c->bloom_fbo[i]);
    glBindTexture(GL_TEXTURE_2D, c->bloom_color[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, c->bloom_width, c->bloom_height,
                 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    tex_params(GL_TEXTURE_2D);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           c->bloom_color[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      fail(c, "HDR bloom framebuffer unavailable");
      goto bad;
    }
  }
  glGenFramebuffers(1, &c->shadow_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, c->shadow_fbo);
  glGenTextures(1, &c->shadow_depth);
  glBindTexture(GL_TEXTURE_2D, c->shadow_depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_SIZE, SHADOW_SIZE,
               0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  tex_params(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         c->shadow_depth, 0);
  GLenum none = GL_NONE;
  glDrawBuffers(1, &none);
  glReadBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fail(c, "Shadow depth framebuffer unavailable");
    goto bad;
  }
  glGenTextures(1, &c->white);
  glBindTexture(GL_TEXTURE_2D, c->white);
  uint8_t white[4] = {255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               white);
  tex_params(GL_TEXTURE_2D);
  glGenTextures(2, c->environment);
  for (int e = 0; e < 2; e++) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, c->environment[e]);
    for (int face = 0; face < 6; face++)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA8, 1, 1, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, white);
    tex_params(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (glGetError() != GL_NO_ERROR) {
    fail(c, "GPU initialization failed");
    goto bad;
  }
  return c;
bad:
  snprintf(global_error, sizeof(global_error), "%s", c->error);
  sg3_free(c);
  return NULL;
}
SG3_API void *sg3_create(int width, int height) {
  return sg3_create_ex(width, height, 1);
}
SG3_API int sg3_set_directional_enabled(void *ctx, int enabled) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  c->directional_enabled = enabled != 0;
  return 1;
}
SG3_API int sg3_samples(void *ctx) {
  Context *c = ctx;
  return c ? c->samples : 0;
}
SG3_API int sg3_set_bloom(void *ctx, double strength) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!isfinite(strength) || strength < 0 || strength > 10)
    return fail(c, "Bloom strength must be0..10");
  c->bloom_strength = (float)strength;
  return 1;
}
SG3_API int sg3_clear_lights(void *ctx) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  memset(c->point_pos_range, 0, sizeof(c->point_pos_range));
  memset(c->point_color_intensity, 0, sizeof(c->point_color_intensity));
  memset(c->spot_color_intensity, 0, sizeof(c->spot_color_intensity));
  c->point_shadow_slot = -1;
  c->spot_shadow_enabled = 0;
  return 1;
}
SG3_API int sg3_point_light_at_ex(void *ctx, int slot, double x, double y,
                                  double z, double r, double g, double b,
                                  double intensity, double range,
                                  int cast_shadows) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  double values[] = {x, y, z};
  if (slot < 0 || slot >= 8 || !finite_array(values, 3, 1e6) || !isfinite(r) ||
      r < 0 || r > 1 || !isfinite(g) || g < 0 || g > 1 || !isfinite(b) ||
      b < 0 || b > 1 || !isfinite(intensity) || intensity < 0 ||
      intensity > 1e9 || !isfinite(range) || range < .001 || range > 1e6)
    return fail(c, "Invalid point light");
  if (cast_shadows && intensity > 0 && c->point_shadow_slot >= 0 &&
      c->point_shadow_slot != slot)
    return fail(c, "Only one shadow-casting point light is supported");
  if (cast_shadows && intensity > 0)
    c->point_shadow_slot = slot;
  else if (c->point_shadow_slot == slot)
    c->point_shadow_slot = -1;
  float *pos = c->point_pos_range + slot * 4,
        *color = c->point_color_intensity + slot * 4;
  pos[0] = (float)x;
  pos[1] = (float)y;
  pos[2] = (float)z;
  pos[3] = (float)range;
  color[0] = (float)r;
  color[1] = (float)g;
  color[2] = (float)b;
  color[3] = (float)intensity;
  return 1;
}
SG3_API int sg3_point_light_at(void *ctx, int slot, double x, double y,
                               double z, double r, double g, double b,
                               double intensity, double range) {
  return sg3_point_light_at_ex(ctx, slot, x, y, z, r, g, b, intensity, range,
                               0);
}
SG3_API int sg3_point_light(void *ctx, double x, double y, double z, double r,
                            double g, double b, double intensity,
                            double range) {
  return sg3_point_light_at(ctx, 0, x, y, z, r, g, b, intensity, range);
}
SG3_API int sg3_spot_light_ex(void *ctx, double x, double y, double z,
                              double dx, double dy, double dz, double r,
                              double g, double b, double intensity,
                              double range, double inner, double outer,
                              int cast_shadows) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  double position[] = {x, y, z}, direction[] = {dx, dy, dz};
  double length = dx * dx + dy * dy + dz * dz;
  if (!finite_array(position, 3, 1e6) || !finite_array(direction, 3, 1e6) ||
      length < 1e-12 || !isfinite(r) || r < 0 || r > 1 || !isfinite(g) ||
      g < 0 || g > 1 || !isfinite(b) || b < 0 || b > 1 ||
      !isfinite(intensity) || intensity < 0 || intensity > 1e9 ||
      !isfinite(range) || range < .001 || range > 1e6 || !isfinite(inner) ||
      !isfinite(outer) || inner < 0 || inner >= outer ||
      outer > 1.5707963267948966)
    return fail(c, "Invalid spot light");
  float *p = c->spot_pos_range, *col = c->spot_color_intensity,
        *dir = c->spot_direction_inner;
  p[0] = (float)x;
  p[1] = (float)y;
  p[2] = (float)z;
  p[3] = (float)range;
  col[0] = (float)r;
  col[1] = (float)g;
  col[2] = (float)b;
  col[3] = (float)intensity;
  length = sqrt(length);
  dir[0] = (float)(dx / length);
  dir[1] = (float)(dy / length);
  dir[2] = (float)(dz / length);
  dir[3] = (float)cos(inner);
  c->spot_outer = (float)cos(outer);
  c->spot_shadow_enabled = cast_shadows && intensity > 0;
  return 1;
}
SG3_API int sg3_spot_light(void *ctx, double x, double y, double z, double dx,
                           double dy, double dz, double r, double g, double b,
                           double intensity, double range, double inner,
                           double outer) {
  return sg3_spot_light_ex(ctx, x, y, z, dx, dy, dz, r, g, b, intensity, range,
                           inner, outer, 0);
}
SG3_API const char *sg3_error(void *ctx) {
  Context *c = ctx;
  return c ? c->error : global_error;
}
SG3_API const char *sg3_renderer(void *ctx) {
  Context *c = ctx;
  return c ? c->renderer : "";
}
SG3_API const char *sg3_version(void *ctx) {
  Context *c = ctx;
  return c ? c->version : "";
}
SG3_API int sg3_is_software(void *ctx) {
  Context *c = ctx;
  return c ? c->software : 0;
}
SG3_API int sg3_mesh(void *ctx, const double *vertices, int count, int stride) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (count < 3 || count > MAX_VERTICES || count % 3 ||
      (stride != 12 && stride != 14) ||
      !finite_array(vertices, count * stride, 1e8)) {
    fail(c, "Invalid GPU vertex buffer");
    return 0;
  }
  if (count > 2000000 - c->total_vertices)
    return fail(c, "Aggregate GPU vertex budget exceeded");
  int slot;
  for (slot = 1; slot < MAX_MESH && c->meshes[slot].vao; slot++) {
  }
  if (slot == MAX_MESH) {
    fail(c, "GPU mesh budget exceeded");
    return 0;
  }
  float *data = malloc((size_t)count * 14 * sizeof(float));
  if (!data) {
    fail(c, "Mesh allocation failed");
    return 0;
  }
  for (int i = 0; i < count; i++) {
    const double *in = vertices + i * stride;
    float *out = data + i * 14;
    for (int j = 0; j < 8; j++)
      out[j] = (float)in[j];
    out[8] = (float)in[stride == 14 ? 8 : 6];
    out[9] = (float)in[stride == 14 ? 9 : 7];
    for (int j = 0; j < 4; j++)
      out[10 + j] = (float)in[(stride == 14 ? 10 : 8) + j];
  }
  Mesh *m = &c->meshes[slot];
  for (int i = 0; i < count; i++)
    for (int j = 0; j < 3; j++)
      m->center[j] += (float)(vertices[i * stride + j] / count);
  glGenVertexArrays(1, &m->vao);
  glBindVertexArray(m->vao);
  glGenBuffers(1, &m->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
  glBufferData(GL_ARRAY_BUFFER,
               (GLsizeiptr)((size_t)count * 14 * sizeof(float)), data,
               GL_STATIC_DRAW);
  free(data);
  int sizes[] = {3, 3, 2, 4, 2};
  int offsets[] = {0, 3, 6, 10, 8};
  for (int a = 0; a < 5; a++) {
    glEnableVertexAttribArray((GLuint)a);
    glVertexAttribPointer((GLuint)a, sizes[a], GL_FLOAT, GL_FALSE,
                          14 * sizeof(float),
                          (void *)(uintptr_t)(offsets[a] * sizeof(float)));
  }
  glBindVertexArray(0);
  if (glGetError() != GL_NO_ERROR) {
    sg3_mesh_free(c, slot);
    fail(c, "GPU vertex upload failed");
    return 0;
  }
  m->count = count;
  c->total_vertices += count;
  return slot;
}
SG3_API int sg3_mesh_free(void *ctx, int mesh) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (mesh < 1 || mesh >= MAX_MESH || !c->meshes[mesh].vao)
    return fail(c, "Invalid mesh ID");
  if (c->begun) {
    for (int i = 0; i < c->draw_count; i++)
      if (c->draws[i].mesh == mesh)
        return fail(c, "Mesh referenced by pending GPU frame");
  }
  c->total_vertices -= c->meshes[mesh].count;
  glDeleteBuffers(1, &c->meshes[mesh].vbo);
  glDeleteVertexArrays(1, &c->meshes[mesh].vao);
  memset(&c->meshes[mesh], 0, sizeof(Mesh));
  return 1;
}
SG3_API int sg3_texture_ex(void *ctx, const uint8_t *rgba, int w, int h,
                           int wrap_s, int wrap_t, int min_filter,
                           int mag_filter) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!rgba || w < 1 || h < 1 || w > 4096 || h > 4096) {
    fail(c, "Invalid texture dimensions");
    return 0;
  }
  if (wrap_s != GL_REPEAT && wrap_s != GL_MIRRORED_REPEAT &&
      wrap_s != GL_CLAMP_TO_EDGE)
    return fail(c, "Invalid wrap S");
  if (wrap_t != GL_REPEAT && wrap_t != GL_MIRRORED_REPEAT &&
      wrap_t != GL_CLAMP_TO_EDGE)
    return fail(c, "Invalid wrap T");
  if (min_filter != GL_NEAREST && min_filter != GL_LINEAR &&
      min_filter != GL_NEAREST_MIPMAP_NEAREST &&
      min_filter != GL_LINEAR_MIPMAP_NEAREST &&
      min_filter != GL_NEAREST_MIPMAP_LINEAR &&
      min_filter != GL_LINEAR_MIPMAP_LINEAR)
    return fail(c, "Invalid min filter");
  if (mag_filter != GL_NEAREST && mag_filter != GL_LINEAR)
    return fail(c, "Invalid mag filter");
  if (w * h > 16777216 - c->total_texture_pixels)
    return fail(c, "Aggregate texture pixel budget exceeded");
  int slot;
  for (slot = 1; slot < MAX_TEXTURE && c->textures[slot]; slot++) {
  }
  if (slot == MAX_TEXTURE)
    return fail(c, "Texture budget exceeded");
  glGenTextures(1, &c->textures[slot]);
  glBindTexture(GL_TEXTURE_2D, c->textures[slot]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               rgba);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
  glGenerateMipmap(GL_TEXTURE_2D);
  if (glGetError() != GL_NO_ERROR) {
    sg3_texture_free(c, slot);
    return fail(c, "Texture upload failed");
  }
  c->texture_pixels[slot] = w * h;
  c->total_texture_pixels += w * h;
  return slot;
}
SG3_API int sg3_texture(void *c, const uint8_t *p, int w, int h) {
  return sg3_texture_ex(c, p, w, h, GL_REPEAT, GL_REPEAT,
                        GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}
SG3_API int sg3_texture_free(void *ctx, int texture) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (texture < 1 || texture >= MAX_TEXTURE || !c->textures[texture])
    return fail(c, "Invalid texture ID");
  if (c->begun) {
    for (int i = 0; i < c->draw_count; i++)
      for (int j = 0; j < 5; j++)
        if (c->draws[i].tex[j] == texture)
          return fail(c, "Texture referenced by pending GPU frame");
  }
  c->total_texture_pixels -= c->texture_pixels[texture];
  c->texture_pixels[texture] = 0;
  glDeleteTextures(1, &c->textures[texture]);
  c->textures[texture] = 0;
  return 1;
}
SG3_API int sg3_environment(void *ctx, const void *diffuse, int diffuse_bytes,
                            const void *specular, int specular_bytes) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!diffuse || !specular || diffuse_bytes != 12288 ||
      specular_bytes != 1048560)
    return fail(c, "Studio cube maps require diffuse16x16x6RGBA16F and "
                   "specular128x128x6RGBA16Fmips8");
  const uint8_t *inputs[] = {diffuse, specular};
  int sizes[] = {16, 128}, levels[] = {1, 8};
  int byte_lengths[] = {diffuse_bytes, specular_bytes};
  for (int map = 0; map < 2; map++) {
    for (int i = 0; i < byte_lengths[map]; i += 2) {
      unsigned half = inputs[map][i] | ((unsigned)inputs[map][i + 1] << 8);
      if ((half & 0x7c00) == 0x7c00)
        return fail(c, "Studio cube maps contain nonfinite half-float samples");
    }
  }
  GLuint staged[2] = {0, 0};
  glGenTextures(2, staged);
  for (int map = 0; map < 2; map++) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, staged[map]);
    size_t off = 0;
    int size = sizes[map];
    for (int level = 0; level < levels[map]; level++) {
      for (int face = 0; face < 6; face++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, level, GL_RGBA16F,
                     size, size, 0, GL_RGBA, GL_HALF_FLOAT, inputs[map] + off);
        off += (size_t)size * size * 8;
      }
      size = size > 1 ? size / 2 : 1;
    }
    tex_params(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    map ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, levels[map] - 1);
  }
  if (glGetError() != GL_NO_ERROR) {
    glDeleteTextures(2, staged);
    return fail(c, "Studio cubemap upload failed");
  }
  glDeleteTextures(2, c->environment);
  memcpy(c->environment, staged, sizeof(staged));
  c->has_environment = 1;
  return 1;
}
SG3_API int sg3_begin(void *ctx, const double *vp, const double *eye,
                      const double *clear, const double *light_vp,
                      const double *light, double exposure) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!finite_array(vp, 16, 1e10) || !finite_array(eye, 3, 1e6) ||
      !finite_array(clear, 4, 1.0) || !finite_array(light_vp, 16, 1e10) ||
      !finite_array(light, 3, 1e6) || !isfinite(exposure) || exposure <= 0 ||
      exposure > 100)
    return fail(c, "Invalid GPU frame parameters");
  double len = light[0] * light[0] + light[1] * light[1] + light[2] * light[2];
  if (len < 1e-12)
    return fail(c, "Light direction cannot be zero");
  for (int i = 0; i < 4; i++)
    if (clear[i] < 0)
      return fail(c, "Clear color channels must be 0..1");
  for (int i = 0; i < 3; i++)
    if (!isfinite((float)eye[i]))
      return fail(c, "Invalid eye");
  copy_float(c->vp, vp, 16);
  copy_float(c->eye, eye, 3);
  copy_float(c->clear, clear, 4);
  copy_float(c->light_vp, light_vp, 16);
  copy_float(c->light, light, 3);
  c->exposure = (float)exposure;
  c->draw_count = 0;
  c->begun = 1;
  c->rendered = 0;
  c->error[0] = 0;
  return 1;
}
SG3_API int sg3_submit_material_ex(
    void *ctx, int mesh, const double *model, const double *base,
    double metallic, double roughness, int bt, int mr, int normal, int ao,
    int em, const double *emissive, int alpha_mode, double cutoff,
    int double_sided, int cast_shadow, const double *uv_selectors,
    double normal_scale, double ao_strength, int unlit) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!c->begun)
    return fail(c, "Begin a GPU frame before submit");
  if (mesh < 1 || mesh >= MAX_MESH || !c->meshes[mesh].vao ||
      !finite_array(model, 16, 1e8) || !finite_array(base, 4, 1.0) ||
      !finite_array(emissive, 3, 100000.0) ||
      !finite_array(uv_selectors, 5, 1.0) || !isfinite(metallic) ||
      metallic < 0 || metallic > 1 || !isfinite(roughness) || roughness < 0 ||
      roughness > 1 || alpha_mode < 0 || alpha_mode > 3 || !isfinite(cutoff) ||
      cutoff < 0 || cutoff > 1000000.0 || !isfinite(normal_scale) ||
      fabs(normal_scale) > 1000000.0 || !isfinite(ao_strength) ||
      ao_strength < 0 || ao_strength > 1)
    return fail(c, "Invalid GPU draw parameters");
  for (int i = 0; i < 4; i++)
    if (base[i] < 0)
      return fail(c, "Base color channels must be 0..1");
  for (int i = 0; i < 3; i++)
    if (emissive[i] < 0)
      return fail(c, "Emissive channels must be nonnegative");
  double det = model[0] * (model[5] * model[10] - model[9] * model[6]) -
               model[4] * (model[1] * model[10] - model[9] * model[2]) +
               model[8] * (model[1] * model[6] - model[5] * model[2]);
  if (fabs(det) < 1e-18)
    return fail(c, "Singular GPU model transform");
  int tex[] = {bt, mr, normal, ao, em};
  for (int i = 0; i < 5; i++) {
    if (tex[i] < 0 || tex[i] >= MAX_TEXTURE || (tex[i] && !c->textures[tex[i]]))
      return fail(c, "Invalid texture ID");
    if (uv_selectors[i] != 0 && uv_selectors[i] != 1)
      return fail(c, "Texture UV set must be 0 or 1");
  }
  if (c->draw_count >= MAX_DRAW)
    return fail(c, "GPU draw budget exceeded");
  Draw *d = &c->draws[c->draw_count++];
  memset(d, 0, sizeof(*d));
  d->mesh = mesh;
  d->mirrored = det < 0;
  copy_float(d->model, model, 16);
  copy_float(d->base, base, 4);
  copy_float(d->emissive, emissive, 3);
  copy_float(d->uv, uv_selectors, 5);
  d->metallic = (float)metallic;
  d->roughness = (float)roughness;
  memcpy(d->tex, tex, sizeof(tex));
  d->alpha_mode = alpha_mode;
  d->cutoff = (float)cutoff;
  d->normal_scale = (float)normal_scale;
  d->ao_strength = (float)ao_strength;
  d->double_sided = double_sided != 0;
  d->cast_shadow = cast_shadow != 0;
  d->unlit = unlit != 0;
  return 1;
}
SG3_API int sg3_submit_material(void *ctx, int mesh, const double *model,
                                const double *base, double metallic,
                                double roughness, int bt, int mr, int normal,
                                int ao, int em, const double *emissive,
                                int alpha_mode, double cutoff, int double_sided,
                                int cast_shadow, const double *uv_selectors,
                                double normal_scale, double ao_strength) {
  return sg3_submit_material_ex(ctx, mesh, model, base, metallic, roughness, bt,
                                mr, normal, ao, em, emissive, alpha_mode,
                                cutoff, double_sided, cast_shadow, uv_selectors,
                                normal_scale, ao_strength, 0);
}
SG3_API int sg3_submit_pbr(void *c, int mesh, const double *model,
                           const double *base, double metallic,
                           double roughness, int bt, int mr, int normal, int ao,
                           int em, const double *emissive, int alpha_mode,
                           double cutoff, int double_sided, int cast_shadow) {
  double uv[5] = {0};
  return sg3_submit_material(c, mesh, model, base, metallic, roughness, bt, mr,
                             normal, ao, em, emissive, alpha_mode, cutoff,
                             double_sided, cast_shadow, uv, 1, 1);
}
SG3_API int sg3_submit(void *c, int mesh, const double *model,
                       const double *base, double metallic, double roughness,
                       int texture, int cast_shadow) {
  double em[3] = {0};
  return sg3_submit_pbr(c, mesh, model, base, metallic, roughness, texture, 0,
                        0, 0, 0, em, 0, 0.5, 1, cast_shadow);
}
static void matrix(GLuint p, const char *n, const float *v) {
  glUniformMatrix4fv(glGetUniformLocation(p, n), 1, GL_FALSE, v);
}
static void uniform1(GLuint p, const char *n, int v) {
  glUniform1i(glGetUniformLocation(p, n), v);
}
static void uniformf(GLuint p, const char *n, float v) {
  glUniform1f(glGetUniformLocation(p, n), v);
}
static void apply(Context *c, GLuint p, Draw *d, int shadow) {
  matrix(p, "model", d->model);
  glUniform4fv(glGetUniformLocation(p, "baseColor"), 1, d->base);
  uniform1(p, "alphaMode", d->alpha_mode);
  uniform1(p, "unlit", d->unlit);
  uniformf(p, "cutoff", d->cutoff);
  glUniform4fv(glGetUniformLocation(p, "uvSelectors"), 1, d->uv);
  uniformf(p, "emissiveUV", d->uv[4]);
  const char *names[] = {"baseMap", "mrMap", "normalMap", "occlusionMap",
                         "emissiveMap"};
  const char *uses[] = {"useBase", "useMR", "useNormal", "useOcclusion",
                        "useEmissive"};
  for (int i = 0; i < (shadow ? 1 : 5); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, d->tex[i] ? c->textures[d->tex[i]] : c->white);
    uniform1(p, names[i], i);
    uniform1(p, uses[i], d->tex[i] != 0);
  }
  if (!shadow) {
    uniformf(p, "metallic", d->metallic);
    uniformf(p, "roughness", d->roughness);
    uniformf(p, "normalScale", d->normal_scale);
    uniformf(p, "aoStrength", d->ao_strength);
    glUniform3fv(glGetUniformLocation(p, "emissive"), 1, d->emissive);
  }
  glFrontFace(d->mirrored ? GL_CW : GL_CCW);
  if (d->double_sided)
    glDisable(GL_CULL_FACE);
  else {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }
  glBindVertexArray(c->meshes[d->mesh].vao);
  glDrawArrays(GL_TRIANGLES, 0, c->meshes[d->mesh].count);
}
static void draw_punctual_depth(void *user, const float *vp) {
  Context *c = user;
  glUseProgram(c->shadow_program);
  matrix(c->shadow_program, "lightVP", vp);
  for (int i = 0; i < c->draw_count; i++)
    if (c->draws[i].cast_shadow && c->draws[i].alpha_mode < 2)
      apply(c, c->shadow_program, &c->draws[i], 1);
}
typedef struct {
  int index;
  float depth;
} DrawOrder;
static int compare_order(const void *a, const void *b) {
  const DrawOrder *x = a, *y = b;
  if (x->depth > y->depth)
    return -1;
  if (x->depth < y->depth)
    return 1;
  return x->index - y->index;
}
static float inverse_tone(float display) {
  double m = pow(fmax(0.0, fmin(1.0, display)), 2.2);
  double a = 2.51 - 2.43 * m, b = .03 - .59 * m;
  return (float)((-b + sqrt(b * b + 4 * a * .14 * m)) / (2 * a));
}
SG3_API int sg3_render(void *ctx) {
  Context *c = ctx;
  if (!active(c))
    return 0;
  if (!c->begun)
    return fail(c, "No GPU frame begun");
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBindFramebuffer(GL_FRAMEBUFFER, c->shadow_fbo);
  glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(c->shadow_program);
  matrix(c->shadow_program, "lightVP", c->light_vp);
  for (int i = 0; i < c->draw_count; i++)
    if (c->directional_enabled && c->draws[i].cast_shadow &&
        c->draws[i].alpha_mode < 2)
      apply(c, c->shadow_program, &c->draws[i], 1);
  int point_slot = c->point_shadow_slot;
  float *point_position = point_slot >= 0 ? c->point_pos_range + point_slot * 4
                                          : c->point_pos_range;
  if (!sg3_shadow_render(&c->punctual_shadows, point_position,
                         point_slot >= 0 ? point_position[3] : 1,
                         point_slot >= 0, c->spot_pos_range,
                         c->spot_direction_inner,
                         c->spot_pos_range[3] > 0 ? c->spot_pos_range[3] : 1,
                         fminf(acosf(c->spot_outer), 1.5706963f),
                         c->spot_shadow_enabled, draw_punctual_depth, c))
    return fail(c, "Punctual shadow map rendering unavailable");
  glBindFramebuffer(GL_FRAMEBUFFER, c->samples == 4 ? c->msaa_fbo : c->fbo);
  glViewport(0, 0, c->width, c->height);
  glClearColor(inverse_tone(c->clear[0]) * c->clear[3],
               inverse_tone(c->clear[1]) * c->clear[3],
               inverse_tone(c->clear[2]) * c->clear[3], c->clear[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(c->program);
  matrix(c->program, "vp", c->vp);
  matrix(c->program, "lightVP", c->light_vp);
  glUniform3fv(glGetUniformLocation(c->program, "eye"), 1, c->eye);
  glUniform3fv(glGetUniformLocation(c->program, "lightDirection"), 1, c->light);
  uniformf(c->program, "exposure", c->exposure);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, c->shadow_depth);
  uniform1(c->program, "shadowMap", 5);
  for (int i = 0; i < 2; i++) {
    glActiveTexture(GL_TEXTURE6 + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, c->environment[i]);
    uniform1(c->program, i ? "specularEnv" : "diffuseEnv", 6 + i);
  }
  uniform1(c->program, "useEnv", c->has_environment);
  uniform1(c->program, "directionalEnabled", c->directional_enabled);
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_CUBE_MAP, c->punctual_shadows.pointTexture
                                         ? c->punctual_shadows.pointTexture
                                         : c->environment[0]);
  uniform1(c->program, "punctualPointMap", 8);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, c->punctual_shadows.spotTexture
                                   ? c->punctual_shadows.spotTexture
                                   : c->white);
  uniform1(c->program, "punctualSpotMap", 9);
  uniform1(c->program, "pointShadowIndex", point_slot);
  uniform1(c->program, "spotShadowEnabled", c->spot_shadow_enabled);
  if (point_slot >= 0) {
    float position_far[4] = {point_position[0], point_position[1],
                             point_position[2], c->punctual_shadows.pointFar};
    glUniform4fv(glGetUniformLocation(c->program, "pointShadowPositionFar"), 1,
                 position_far);
    uniformf(c->program, "pointShadowNear", c->punctual_shadows.pointNear);
  }
  matrix(c->program, "spotLightVP", c->punctual_shadows.spotVP);

  glUniform4fv(glGetUniformLocation(c->program, "pointPosRange"), 8,
               c->point_pos_range);
  glUniform4fv(glGetUniformLocation(c->program, "pointColorIntensity"), 8,
               c->point_color_intensity);
  glUniform4fv(glGetUniformLocation(c->program, "spotPosRange"), 1,
               c->spot_pos_range);
  glUniform4fv(glGetUniformLocation(c->program, "spotColorIntensity"), 1,
               c->spot_color_intensity);
  glUniform4fv(glGetUniformLocation(c->program, "spotDirectionInner"), 1,
               c->spot_direction_inner);
  uniformf(c->program, "spotOuter", c->spot_outer);
  DrawOrder blended[MAX_DRAW];
  int blends = 0;
  for (int i = 0; i < c->draw_count; i++) {
    Draw *d = &c->draws[i];
    if (d->alpha_mode < 2) {
      apply(c, c->program, d, 0);
      continue;
    }
    Mesh *m = &c->meshes[d->mesh];
    float depth = 0;
    for (int j = 0; j < 3; j++) {
      float v = d->model[j] * m->center[0] + d->model[4 + j] * m->center[1] +
                d->model[8 + j] * m->center[2] + d->model[12 + j] - c->eye[j];
      depth += v * v;
    }
    blended[blends++] = (DrawOrder){i, depth};
  }
  qsort(blended, (size_t)blends, sizeof(DrawOrder), compare_order);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                      GL_ONE_MINUS_SRC_ALPHA);
  glDepthMask(GL_FALSE);
  for (int i = 0; i < blends; i++) {
    Draw *d = &c->draws[blended[i].index];
    glBlendFuncSeparate(GL_SRC_ALPHA,
                        d->alpha_mode == 3 ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    apply(c, c->program, d, 0);
  }
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBindVertexArray(0);
  if (c->samples == 4) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, c->msaa_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, c->fbo);
    glBlitFramebuffer(0, 0, c->width, c->height, 0, 0, c->width, c->height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
  }
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  if (c->bloom_strength > 0) {
    glUseProgram(c->bloom_program);
    uniform1(c->bloom_program, "sourceMap", 0);
    glBindVertexArray(c->tone_vao);
    for (int pass = 0; pass < 2; pass++) {
      glBindFramebuffer(GL_FRAMEBUFFER, c->bloom_fbo[pass]);
      glViewport(0, 0, c->bloom_width, c->bloom_height);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, pass ? c->bloom_color[0] : c->color);
      float step[4] = {pass ? 0.0f : 1.0f / c->bloom_width,
                       pass ? 1.0f / c->bloom_height : 0.0f, pass ? 0.0f : 1.0f,
                       0};
      glUniform4fv(glGetUniformLocation(c->bloom_program, "stepAndExtract"), 1,
                   step);
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, c->output_fbo);
  glViewport(0, 0, c->width, c->height);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glUseProgram(c->tone_program);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, c->color);
  uniform1(c->tone_program, "sceneMap", 0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,
                c->bloom_strength > 0 ? c->bloom_color[1] : c->white);
  uniform1(c->tone_program, "bloomMap", 1);
  uniformf(c->tone_program, "bloomStrength", c->bloom_strength);
  glBindVertexArray(c->tone_vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, c->width, c->height, GL_RGBA, GL_UNSIGNED_BYTE, c->bottom);
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    char message[128];
    snprintf(message, sizeof(message), "GPU render failed GL error 0x%x",
             error);
    return fail(c, message);
  }
  size_t row = (size_t)c->width * 4;
  for (int y = 0; y < c->height; y++)
    memcpy(c->pixels + (size_t)y * row,
           c->bottom + (size_t)(c->height - 1 - y) * row, row);
  for (size_t i = 0, n = (size_t)c->width * c->height; i < n; i++) {
    uint8_t *p = c->pixels + i * 4;
    if (p[3] > 0 && p[3] < 255) {
      for (int j = 0; j < 3; j++) {
        unsigned v = ((unsigned)p[j] * 255 + p[3] / 2) / p[3];
        p[j] = (uint8_t)(v > 255 ? 255 : v);
      }
    } else if (p[3] == 0)
      p[0] = p[1] = p[2] = 0;
  }
  c->rendered = 1;
  c->begun = 0;
  return 1;
}
SG3_API const uint8_t *sg3_pixels(void *ctx) {
  Context *c = ctx;
  return c && c->rendered ? c->pixels : NULL;
}
SG3_API int sg3_finish(void *ctx, uint8_t *rgba, int capacity) {
  Context *c = ctx;
  if (!c || !rgba || capacity < c->width * c->height * 4)
    return fail(c, "Output RGBA buffer too small");
  if (!sg3_render(c))
    return 0;
  memcpy(rgba, c->pixels, (size_t)c->width * c->height * 4);
  return c->width * c->height * 4;
}
SG3_API void sg3_free(void *ctx) {
  Context *c = ctx;
  if (!c)
    return;
  int current = c->platform && sg_platform_make_current(c->platform);
  if (current) {
    for (int i = 1; i < MAX_MESH; i++) {
      if (c->meshes[i].vbo)
        glDeleteBuffers(1, &c->meshes[i].vbo);
      if (c->meshes[i].vao)
        glDeleteVertexArrays(1, &c->meshes[i].vao);
    }
    for (int i = 1; i < MAX_TEXTURE; i++)
      if (c->textures[i])
        glDeleteTextures(1, &c->textures[i]);
    sg3_shadow_free(&c->punctual_shadows);
    if (c->bloom_program)
      glDeleteProgram(c->bloom_program);
    glDeleteTextures(2, c->bloom_color);
    glDeleteFramebuffers(2, c->bloom_fbo);
    glDeleteFramebuffers(1, &c->msaa_fbo);
    glDeleteRenderbuffers(1, &c->msaa_color);
    glDeleteRenderbuffers(1, &c->msaa_depth);
    if (c->tone_program)
      glDeleteProgram(c->tone_program);
    glDeleteVertexArrays(1, &c->tone_vao);
    glDeleteFramebuffers(1, &c->output_fbo);
    glDeleteTextures(1, &c->output_color);
    if (c->program)
      glDeleteProgram(c->program);
    if (c->shadow_program)
      glDeleteProgram(c->shadow_program);
    glDeleteTextures(2, c->environment);
    glDeleteTextures(1, &c->white);
    glDeleteTextures(1, &c->color);
    glDeleteTextures(1, &c->depth);
    glDeleteTextures(1, &c->shadow_depth);
    glDeleteFramebuffers(1, &c->fbo);
    glDeleteFramebuffers(1, &c->shadow_fbo);
  }
  if (c->platform)
    sg_platform_free(c->platform);
  free(c->pixels);
  free(c->bottom);
  free(c);
}
