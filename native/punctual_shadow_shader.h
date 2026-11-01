#ifndef SG3_PUNCTUAL_SHADOW_SHADER_H
#define SG3_PUNCTUAL_SHADOW_SHADER_H
#if defined(__APPLE__) || defined(_WIN32)
#define SG3_SHADOW_SAMPLER_PRECISION ""
#else
#define SG3_SHADOW_SAMPLER_PRECISION "highp "
#endif
/* Cube comparisons intersect each sample ray with the receiver plane, avoiding
 * the same slope acne that a center-depth-only PCF creates on planar floors. */
#define SG3_PUNCTUAL_SHADOW_GLSL                                               \
  "uniform " SG3_SHADOW_SAMPLER_PRECISION                                      \
  "samplerCube punctualPointMap;uniform " SG3_SHADOW_SAMPLER_PRECISION         \
  "sampler2D "                                                                 \
  "punctualSpotMap;uniform vec4 pointShadowPositionFar;uniform float "         \
  "pointShadowNear;uniform mat4 spotLightVP;uniform int "                      \
  "pointShadowIndex;uniform int spotShadowEnabled;\n"                          \
  "vec3 sg3_cubeRay(int face,vec2 uv){if(face==0)return "                      \
  "vec3(1.0,-uv.y,-uv.x);if(face==1)return "                                   \
  "vec3(-1.0,-uv.y,uv.x);if(face==2)return "                                   \
  "vec3(uv.x,1.0,uv.y);if(face==3)return "                                     \
  "vec3(uv.x,-1.0,-uv.y);if(face==4)return vec3(uv.x,-uv.y,1.0);return "       \
  "vec3(-uv.x,-uv.y,-1.0);}\n"                                                 \
  "float sg3_point_visibility(vec3 position,vec3 n,vec3 l){vec3 "              \
  "delta=position-pointShadowPositionFar.xyz;vec3 ad=abs(delta);float "        \
  "major=max(ad.x,max(ad.y,ad.z));if(major<=pointShadowNear||major>="          \
  "pointShadowPositionFar.a)return 1.0;int face;vec2 "                         \
  "uv;if(ad.x>=ad.y&&ad.x>=ad.z){face=delta.x>0.0?0:1;uv=vec2(delta.x>0.0?-"   \
  "delta.z:delta.z,-delta.y)/ad.x;}else "                                      \
  "if(ad.y>=ad.z){face=delta.y>0.0?2:3;uv=vec2(delta.x,delta.y>0.0?delta.z:-"  \
  "delta.z)/"                                                                  \
  "ad.y;}else{face=delta.z>0.0?4:5;uv=vec2(delta.z>0.0?delta.x:-delta.x,-"     \
  "delta.y)/ad.z;}vec2 center=floor((uv*.5+.5)*1024.0)+vec2(.5);float "        \
  "numerator=dot(n,delta);float "                                              \
  "bias=max(.0005,major*.0006*(1.0-max(dot(n,l),0.0)));float "                 \
  "visibility=0.0;for(int x=-1;x<=1;x++)for(int y=-1;y<=1;y++){vec2 "          \
  "sampleUV=(center+vec2(float(x),float(y)))/1024.0*2.0-1.0;vec3 "             \
  "ray=sg3_cubeRay(face,sampleUV);float denominator=dot(n,ray);float "         \
  "sampleMajor=max(abs(ray.x),max(abs(ray.y),abs(ray.z)));float "              \
  "expected=abs(denominator)>1e-7?numerator/"                                  \
  "denominator*sampleMajor:major;float "                                       \
  "depth=texture(punctualPointMap,ray).r;float "                               \
  "farPlane=pointShadowPositionFar.a;float "                                   \
  "stored=(2.0*pointShadowNear*farPlane)/"                                     \
  "(farPlane+pointShadowNear-(depth*2.0-1.0)*(farPlane-pointShadowNear));"     \
  "visibility+=(expected-bias<=stored)?1.0:0.0;}return visibility/9.0;}\n"     \
  "float sg3_spot_visibility(vec3 position,vec3 n,vec3 l){vec4 "               \
  "projected=spotLightVP*vec4(position,1.0);vec3 "                             \
  "q=projected.xyz/projected.w*.5+.5;vec3 "                                    \
  "dx=dFdx(q),dy=dFdy(q);if(projected.w<=0.0||q.x<0.0||q.x>1.0||q.y<0.0||q.y>" \
  "1.0||q.z<0.0||q.z>1.0)return 1.0;float "                                    \
  "determinant=dx.x*dy.y-dx.y*dy.x;vec2 "                                      \
  "gradient=abs(determinant)>1e-12?vec2(dy.y*dx.z-dx.y*dy.z,dx.x*dy.z-dy.x*"   \
  "dx.z)/determinant:vec2(0.0);float "                                         \
  "bias=max(.0004*(1.0-max(dot(n,l),0.0)),.00008);float "                      \
  "visibility=0.0;for(int x=-1;x<=1;x++)for(int y=-1;y<=1;y++){vec2 "          \
  "center=(floor(q.xy*1024.0)+vec2(.5)+vec2(float(x),float(y)))/1024.0;float " \
  "stored=texture(punctualSpotMap,center).r;visibility+=(q.z+dot(gradient,"    \
  "center-q.xy)-bias<=stored)?1.0:0.0;}return visibility/9.0;}\n"
#endif
