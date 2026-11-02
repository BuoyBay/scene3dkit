# Scene3DKit

Scene3DKit provides native GPU and CPU 3D scenes written for Buoy. Import
`scene3dkit.Scene3DKit` for meshes, actors and cameras, `scene3dkit.GPU` for the
GPU renderer, and `scene3dkit.GLTF` for glTF 2 binary models. Use these qualified
imports consistently when combining modules.

The GPU path uses an offscreen OpenGL/GLES context with retained vertex buffers,
per-pixel depth, GGX metallic/roughness shading, smooth normals, normal maps,
base-color/metallic-roughness/occlusion/emissive textures, two UV sets, HDR studio
cubemap reflections, HDR bloom, multisample resolve, and bounded punctual lighting
and shadow maps. Native Canvas composition uses
a copied `Picture` readback of the actual GPU framebuffer. Linux uses EGL/GLES3,
macOS CGL/OpenGL3.2, and Windows WGL/OpenGL3.3. No browser or Rust engine is used.

`GPUScene3D(width,height,samples=4)` opens a context on the caller thread; dimensions are
1..4096. The context, rendering, cache operations and explicit `Close()` must stay
on that thread. `Samples` reports the actual sample count separately from the
request: unsupported4x targets fall back to1sample. `Renderer`, `Version` and
`IsSoftware` report the actual backend.
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
compatibility directional shadow map covers a fixed studio volume of approximately
+/-12 world units. Punctual shadows use the actual light position/direction/range
and separate bounded point-cube/spot depth maps.

The shader approximates the source game's PBR studio, without claiming pixel
identity with Bevy. Blended draw groups sort by their world-space center distance;
intersecting transparent surfaces still need a more advanced transparency
technique. Animation/skinning, arbitrary post-processing chains and arbitrary
environment formats are not implemented.
GPU readback has a cost; this API favors safe native Canvas integration over a
platform-specific embedded swapchain surface.

`Material3D` contains `BaseColor` (white), `Metallic`/`Roughness` (1), `Emissive`
(black), `NormalScale` (1), `OcclusionStrength` (1), `AlphaMode` (uppercase OPAQUE,
MASK, BLEND or runtime-only ADD), `AlphaCutoff` (.5), and `DoubleSided` (False). The five texture
properties are `BaseColorTexture`, `MetallicRoughnessTexture`, `NormalTexture`,
`OcclusionTexture` and `EmissiveTexture`, each with an equivalent `*TexCoord`
property selecting UV0 or UV1. `Texture3D(image)` retains a Picture and glTF sampler
properties. `EmissiveStrength` (default1, finite0..1000) scales linear emission beyond the
normalized Color channels. `Unlit` (defaultFalse) bypasses direct/ambient PBR
lighting while retaining base color, textures and emission. `Validate()` rejects
nonfinite/out-of-bounds material values.

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

Verified native GPU tests include Linux Mesa llvmpipe software execution and
macOS Apple M4 Pro hardware execution. Cross-target compiler checks are not
native target execution. GPU timing depends on the backend, scene complexity, viewport size, upload work and readback.

The GPU scene accumulates linear HDR color in an RGBA16F framebuffer before a
fullscreen exposure/ACES/gamma pass produces the readback image. Runtime ADD
materials use source-alpha additive RGB blending with depth testing enabled and
depth writes disabled; coincident layers accumulate before tonemapping. The
standard glTF loader still accepts only OPAQUE, MASK and BLEND. Independent tests
compare ADD alpha0/.5/1 and HDR sums against explicit tone-map calculations, and
verify that unlit rendering is independent of surface normals. This implements
the source flame's additive HDR layers. Bloom and multisample resolve operate on
the same HDR scene before the final display conversion.

`BloomStrength` (default0, finite0..10) controls HDR bright extraction and
separable Gaussian glow before tone mapping. It approximates the authored Bevy
Bloom::NATURAL visual intent rather than reproducing its exact multiscale kernel.
The application sets0.12 for Balloon and0.22 for Flame.

`PointLights` accepts at most8 `PointLight3D` objects; `SpotLight` accepts one
`SpotLight3D`. Each light exposes `Position`, linear-light `Color`, physical-style
`Intensity` (0..1e9), `Range` (.001..1e6), and `CastShadows` (False). A spotlight
also has nonzero `Direction` and cone angles `0 <= InnerAngle < OuterAngle <= pi/2`.
There can be at most one shadow-casting point light. `Shadows` is the master
switch. `DirectionalLightEnabled` defaults to True for the generic studio
compatibility light; Mind Games sets it False because its source scenes use the
authored punctual lights and environment rather than an extra directional light. Punctual lights illuminate nonemissive materials with inverse-square
attenuation and smooth range/cone falloff; shadow-casting uses a bounded1024pixel
six-face point depth cube and1024pixel spot perspective map. These are hard/PCF
shadows, with no claim of area-light radius or soft-shadow identity with Bevy.
The legacy `PointLight`, `PointColor`, `PointIntensity`, `PointRange` properties
remain a slot0 convenience when `PointLights` is empty. Light uniforms are updated
every render, so source fire intensity/flicker does not require mesh uploads.

The complete authored camera/light contract and verification requirements are in
[SOURCE-RENDERING-CONTRACT.md](../SOURCE-RENDERING-CONTRACT.md). Renderer capability
and native evidence must be checked independently of requested effects.

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

The static glTF 2 binary importer lives in `scene3dkit.GLTF`; import that module
alongside `scene3dkit` to use `LoadGLB(path, triangleStride=1)` or
`DecodeGLB(bytes, triangleStride=1)`. It bakes selected-scene node transforms and
base color into meshes. See [GLTF.md](../GLTF.md) for its supported subset and bounds.
Triangle stride is explicit
sampling that leaves holes; use closed simplified meshes for interactive drawing.

Embedded PNG/JPEG decoding requires a separately built `buoy_scene3dimages` runtime. This source distribution does not include those decoder slices; run `python3 tools/build_scene3dimages.py` for supported targets before packaging. Missing decoders produce an explicit unsupported-operation error.

Builder context: run shared `tools/` commands from the assembled
[BuoyBay workspace](https://github.com/BuoyBay/workspace-tools). The scripts live
in [devtools](https://github.com/BuoyBay/devtools), alongside the shared build inputs.
