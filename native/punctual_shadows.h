#ifndef SG3_PUNCTUAL_SHADOWS_H
#define SG3_PUNCTUAL_SHADOWS_H
#include "platform.h"
#include <math.h>
#include <string.h>
#define SG3_PUNCTUAL_SHADOW_SIZE 1024
/* One point cube and one spot map per context. Allocation is lazy and bounded.
 */
typedef struct {
  GLuint pointTexture, pointFBO, spotTexture, spotFBO;
  float spotVP[16], pointNear, pointFar;
} SG3Shadows;
typedef void (*SG3ShadowDraw)(void *user, const float *viewProjection);
static void sg3_shadow_init(SG3Shadows *state) {
  memset(state, 0, sizeof(*state));
}
static void sg3_shadow_free(SG3Shadows *state) {
  glDeleteTextures(1, &state->pointTexture);
  glDeleteTextures(1, &state->spotTexture);
  glDeleteFramebuffers(1, &state->pointFBO);
  glDeleteFramebuffers(1, &state->spotFBO);
  sg3_shadow_init(state);
}
static void sg3_shadow_cross(const float *a, const float *b, float *out) {
  out[0] = a[1] * b[2] - a[2] * b[1];
  out[1] = a[2] * b[0] - a[0] * b[2];
  out[2] = a[0] * b[1] - a[1] * b[0];
}
static void sg3_shadow_normalize(float *v) {
  float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (length > 1e-12f)
    for (int i = 0; i < 3; i++)
      v[i] /= length;
}
static void sg3_shadow_matrix(const float *position, const float *direction,
                              const float *up, float tangent, float nearPlane,
                              float farPlane, float *out) {
  float f[3] = {direction[0], direction[1], direction[2]}, s[3], u[3];
  sg3_shadow_normalize(f);
  sg3_shadow_cross(f, up, s);
  sg3_shadow_normalize(s);
  sg3_shadow_cross(s, f, u);
  float view[16] = {s[0], u[0], -f[0], 0, s[1], u[1], -f[1], 0,
                    s[2], u[2], -f[2], 0, 0,    0,    0,     1};
  for (int j = 0; j < 3; j++) {
    view[12] -= s[j] * position[j];
    view[13] -= u[j] * position[j];
    view[14] += f[j] * position[j];
  }
  float projection[16] = {0};
  projection[0] = projection[5] = 1.0f / tangent;
  projection[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
  projection[11] = -1.0f;
  projection[14] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
  for (int col = 0; col < 4; col++)
    for (int row = 0; row < 4; row++) {
      out[col * 4 + row] = 0;
      for (int k = 0; k < 4; k++)
        out[col * 4 + row] += projection[k * 4 + row] * view[col * 4 + k];
    }
}
static int sg3_shadow_allocate(GLuint *texture, GLuint *fbo, int cube) {
  if (*texture)
    return 1;
  GLuint stagedTexture = 0, stagedFBO = 0;
  GLenum target = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
  glGenTextures(1, &stagedTexture);
  glBindTexture(target, stagedTexture);
  for (int face = 0; face < (cube ? 6 : 1); face++)
    glTexImage2D(cube ? GL_TEXTURE_CUBE_MAP_POSITIVE_X + face : GL_TEXTURE_2D,
                 0, GL_DEPTH_COMPONENT24, SG3_PUNCTUAL_SHADOW_SIZE,
                 SG3_PUNCTUAL_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT,
                 GL_UNSIGNED_INT, NULL);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (cube)
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glGenFramebuffers(1, &stagedFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, stagedFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         cube ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_2D,
                         stagedTexture, 0);
  GLenum none = GL_NONE;
  glDrawBuffers(1, &none);
  glReadBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
      glGetError() != GL_NO_ERROR) {
    glDeleteTextures(1, &stagedTexture);
    glDeleteFramebuffers(1, &stagedFBO);
    return 0;
  }
  *texture = stagedTexture;
  *fbo = stagedFBO;
  return 1;
}
static int sg3_shadow_render(SG3Shadows *state, const float *pointPosition,
                             float pointRange, int pointEnabled,
                             const float *spotPosition,
                             const float *spotDirection, float spotRange,
                             float outerAngle, int spotEnabled,
                             SG3ShadowDraw draw, void *user) {
  if (!state || !draw)
    return 0;
  if (pointEnabled) {
    if (!pointPosition || !isfinite(pointRange) || pointRange <= 0.0f)
      return 0;
    for (int i = 0; i < 3; i++)
      if (!isfinite(pointPosition[i]))
        return 0;
  }
  if (spotEnabled) {
    if (!spotPosition || !spotDirection || !isfinite(spotRange) ||
        spotRange <= 0.0f || !isfinite(outerAngle) || outerAngle <= 0.0f ||
        outerAngle >= 1.5707963267948966f)
      return 0;
    float length = 0.0f;
    for (int i = 0; i < 3; i++) {
      if (!isfinite(spotPosition[i]) || !isfinite(spotDirection[i]))
        return 0;
      length += spotDirection[i] * spotDirection[i];
    }
    if (!isfinite(length) || length < 1e-12f)
      return 0;
  }
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glViewport(0, 0, SG3_PUNCTUAL_SHADOW_SIZE, SG3_PUNCTUAL_SHADOW_SIZE);
  if (pointEnabled) {
    if (!pointPosition || !isfinite(pointRange) || pointRange <= 0)
      return 0;
    if (!sg3_shadow_allocate(&state->pointTexture, &state->pointFBO, 1))
      return 0;
    state->pointNear = fminf(.05f, pointRange * .01f);
    state->pointFar = pointRange;
    static const float directions[6][3] = {{1, 0, 0},  {-1, 0, 0}, {0, 1, 0},
                                           {0, -1, 0}, {0, 0, 1},  {0, 0, -1}};
    static const float ups[6][3] = {{0, -1, 0}, {0, -1, 0}, {0, 0, 1},
                                    {0, 0, -1}, {0, -1, 0}, {0, -1, 0}};
    glBindFramebuffer(GL_FRAMEBUFFER, state->pointFBO);
    for (int face = 0; face < 6; face++) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                             state->pointTexture, 0);
      glClear(GL_DEPTH_BUFFER_BIT);
      float vp[16];
      sg3_shadow_matrix(pointPosition, directions[face], ups[face], 1.0f,
                        state->pointNear, pointRange, vp);
      draw(user, vp);
    }
  }
  if (spotEnabled) {
    if (!spotPosition || !spotDirection || !isfinite(spotRange) ||
        spotRange <= 0 || !isfinite(outerAngle) || outerAngle <= 0 ||
        outerAngle >= 1.5707963267948966f)
      return 0;
    if (!sg3_shadow_allocate(&state->spotTexture, &state->spotFBO, 0))
      return 0;
    float up[3] = {0, 1, 0};
    float normalized[3] = {spotDirection[0], spotDirection[1],
                           spotDirection[2]};
    sg3_shadow_normalize(normalized);
    if (fabsf(normalized[1]) > .98f) {
      up[1] = 0;
      up[2] = 1;
    }
    sg3_shadow_matrix(spotPosition, normalized, up, tanf(outerAngle),
                      fminf(.05f, spotRange * .01f), spotRange, state->spotVP);
    glBindFramebuffer(GL_FRAMEBUFFER, state->spotFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    draw(user, state->spotVP);
  }
  return glGetError() == GL_NO_ERROR;
}
#endif
