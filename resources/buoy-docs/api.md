---
title: "scene3dkit API contracts"
summary: "Usage and behavioral contracts for scene3dkit."
category: "Reference"
symbols: ["GPUScalar3D", "GPUIndex3D", "GPUVertex3D", "GPUScene3D", "Vec3D", "Texture3D", "Material3D", "Triangle3D", "Mesh3D", "MeshActor3D", "Camera3D", "Scene3D"]
related: ["index.md", "getting-started.md", "typed-api.md"]
---

# Scene3DKit

Scene3DKit provides native GPU and CPU 3D scenes written for Buoy. Import
`scene3dkit.Scene3DKit` for meshes, actors and cameras, `scene3dkit.GPU` for the
GPU renderer, and `scene3dkit.GLTF` for glTF 2 binary models. Use these qualified
imports consistently when combining modules.

The GPU path uses an offscreen OpenGL/GLES context with retained vertex buffers,
per-pixel depth, GGX metallic/roughness shading, smooth normals, normal maps,
base-color/metallic-roughness/occlusion/emissive textures, two UV sets, HDR studio
cubemap reflections and a directional shadow map. Native Canvas composition uses
a copied `Picture` readback of the actual GPU framebuffer. Linux uses EGL/GLES3,
macOS CGL/OpenGL3.2, and Windows WGL/OpenGL3.3. No browser or Rust engine is used.

`GPUScene3D(width,height)` opens a context on the caller thread; dimensions are
1..4096. The context, rendering, cache operations and explicit `Close()` must stay
on that thread. `Renderer`, `Version` and `IsSoftware` report the actual backend.
Mesa llvmpipe is software GPU execution and is reported as such. Context creation
may throw when the native runtime or graphics driver is unavailable; applications
can retain the CPU renderer as a labeled fallback.

`gpu.Render(scene,camera)` returns an owned `Picture`. `ClearColor` defaults to
transparent; `Exposure` is positive and at most 100; `Shadows` defaults to True.
`SetEnvironment(diffuseBytes,specularBytes)` accepts the original Mind Games raw
RGBA16F cube layouts: diffuse16x16x6 (12,288 bytes), specular128x128x6 with8mips
(1,048,560 bytes). Inputs are validated before replacement. The renderer has a
procedural studio environment until explicit maps are supplied.

GPU uploads are retained by mesh/material groups, and textures are cached by
identity. Replacing material scalar values does not reupload geometry. Use
`InvalidateMesh(mesh)` after changing an existing triangle's normal, UV, tint or
material reference, and `InvalidateTexture(texture)` after changing pixels or
sampler settings. `ClearCache()` drops all uploaded resources. A changed mesh
triangle count also causes reupload. `CachedMeshes` and `CachedTextures` expose
cache counts. Callers should invalidate obsolete objects during reload rather
than retaining a growing history. `Close()` is idempotent; use after close throws.

A GPU context holds at most511 mesh buffers,255 textures,2,000,000 aggregate
uploaded vertices and16,777,216 aggregate texture pixels; a draw pass accepts at
most4096 batches. Texture dimensions are at most4096. Rendering validates finite
scalars, matrix bounds and resource identities before native submission. The
shadow map covers a fixed studio volume of approximately +/-12 world units;
applications outside it need a configurable lighting extension.

The shader approximates the source game's PBR studio, without claiming pixel
identity with Bevy. Blended draw groups sort by their world-space center distance;
intersecting transparent surfaces still need a more advanced transparency
technique. Point lights, scene-wide tone-mapping controls, animation/skinning,
post-processing effects and arbitrary environment formats are not implemented.
GPU readback has a cost; this API favors safe native Canvas integration over a
platform-specific embedded swapchain surface.

`Material3D` contains `BaseColor` (white), `Metallic`/`Roughness` (1), `Emissive`
(black), `NormalScale` (1), `OcclusionStrength` (1), `AlphaMode` (uppercase OPAQUE,
MASK or BLEND), `AlphaCutoff` (.5), and `DoubleSided` (False). The five texture
properties are `BaseColorTexture`, `MetallicRoughnessTexture`, `NormalTexture`,
`OcclusionTexture` and `EmissiveTexture`, each with an equivalent `*TexCoord`
property selecting UV0 or UV1. `Texture3D(image)` retains a Picture and glTF sampler
properties. `Validate()` rejects nonfinite/out-of-bounds material values.

`Mesh3D.AddSurfaceTriangle(a,b,c,na,nb,nc,uv0a,uv0b,uv0c,uv1a,uv1b,uv1c,material)`
adds surface metadata while `AddTriangle(a,b,c[,tint])` retains its simpler API.
Triangle vertices are immutable; normal/UV/material metadata is mutable. The GPU
computes geometric normals when no normals are supplied. `MeshActor3D.MaterialOverride`
replaces an imported material for that actor, supporting palette changes without
throwing away the original mesh. Actor `Metallic`/`Roughness` apply to simple
triangles without a material. `SetMesh(mesh)` replaces geometry while preserving
actor identity and transforms.

The native bridge and bounded PNG/JPEG decoder build from source with
`python3 tools/build_scene3dkit.py --verify`. The builder stages current sources
and freshly built native runtime slices before sealing. It supports explicit
`--target`/`--cc` for cross-compilation; GPU availability is tested when a context
is opened, independently of compilation.

Historical native GPU tests covered Linux Mesa llvmpipe and macOS Apple M4 Pro,
but their recorded native source hashes do not match this source snapshot.
The packaged runtime slices are freshly built x86_64 Linux and cross-built
x86_64 Windows; neither GPU path was executed for this package. macOS and ARM
runtime slices are not included and require builds from matching sources.
Cross-target compiler checks and Windows cross-compilation do not establish
native target execution. Run `tests/scene3dkit_gpu_verify.py` for the public Buoy
consumer, and `tests/scene3dkit_gpu_review.py` for independent native pixel/lifecycle
oracles. `tests/scene3dkit_gpu_benchmark.py` renders all57,436 triangles of each
original balloon with studio reflections and retains PNGs/timings plus the actual
backend string. On the sampled Linux software GPU, warm960x640frames took about
43–49ms including shadows and readback; cold upload and GLB parsing are separate.

The CPU fallback API and bounds follow below.

`Scene3D.Add(actor)` retains a `MeshActor3D`; `Remove(actor)` and `Clear()` release
membership. Duplicate actors reject, and a scene holds at most 256 actors.
`Draw(g, camera, width, height)` draws without clearing, clips the viewport, and
restores graphics state even if a drawing operation throws. Drawing never
advances game state. `LastDrawnFaces` counts submitted clipped polygons.

`MeshActor3D(mesh, tint)` shares its mesh and exposes `X/Y/Z`, `ScaleX/Y/Z`,
`RotationY` (radians), `Visible`, `Tint` and `Metallic` (0..1). Transform order is
scale, Y rotation, translation. Negative scales are permitted. `Transform(point)`
returns a new world point. `Validate()` checks actor state; scene drawing validates
all visible actors before drawing. Scene coordinates and scales are bounded to
10,000 in magnitude, rotations to 1,000,000 radians. Colors must have finite
channels in 0..1. Calls belong on one owning thread.

`Camera3D` defaults to distance 6, target Y 0, focal length 0.95 and near plane
0.05. `Orbit(yaw, pitch)` uses radians and clamps pitch to +/-1.5 to avoid camera
inversion. `View(point)` returns camera coordinates, where positive Z is depth.
Distance is 0.1..10,000, near plane 0.001..100, focal length 0.1..10.
Viewport dimensions must be positive and at most 16,384. Positive world Y points
up; positive world Z faces the camera at zero orbit.

`Mesh3D.AddTriangle(a,b,c[,tint])` takes immutable `Vec3D(x,y,z)` vertices.
`Count` counts triangles; `Triangles()` returns a defensive list snapshot.
`Triangle3D` vertices and tint are read-only. A per-triangle tint overrides the
actor's tint. Meshes are mutable and shared; additions affect all actors using
them. Limits are 131,072 triangles per mesh and 262,144 per visible scene.
Vertices are finite and bounded to 1,000,000 in magnitude; transformed and camera
points must fit the same bound.

The primitive factories return fresh meshes:

- `BalloonMesh3D(shape=0, segments=32, rings=20)`: round(0), heart(1), star(2).
  Radius is approximately one; round envelopes are slightly taller and flatter.
- `SphereMesh3D(segments=24,rings=16)`: unit sphere.
- `CylinderMesh3D(segments=24)`: radius one, Y from -1 to +1, capped.

Segments must be 8..64 and rings 4..64. Sphere/balloon factories emit
`2*segments*rings` triangles including degenerate pole faces, which the renderer
skips. These are procedural envelopes, with no claim of matching authored foil
seams or Blender topology.

Triangles are clipped against the near plane and four viewport frustum planes
before projection, preventing unbounded native path coordinates. Faces are ordered from far to near
by average vertex depth, then filled with native paths. This painter algorithm
works for the application's simple separated objects; intersecting triangles,
cyclic overlaps and transparent surfaces may sort incorrectly. The CPU fallback has no
per-pixel depth buffer, textures, PBR, shadows or environment maps. The GPU path
provides those material/depth features as described above. Large imported meshes
can be expensive; callers should choose a deliberate triangle budget.

Maintainers run the private validation suite before release. See the repository README for public build instructions.


The static glTF 2 binary importer lives in `scene3dkit.GLTF`; import that module
alongside `scene3dkit` to use `LoadGLB(path, triangleStride=1)` or
`DecodeGLB(bytes, triangleStride=1)`. It bakes selected-scene node transforms and
base color into meshes. See GLTF.md for its supported subset and bounds.
`tests/scene3dkit_gltf_verify.py` checks original/CPU assets and malformed fixtures;
`tests/scene3dkit_gltf_security_verify.py` independently challenges hostile inputs
and signed nested transforms. A local render benchmark and PNG evidence are
reproducible with `tests/scene3dkit_benchmark.py`. Triangle stride is explicit
sampling that leaves holes; use closed simplified meshes for interactive drawing.


[Typed API](typed-api.md) · [Getting started](getting-started.md)
