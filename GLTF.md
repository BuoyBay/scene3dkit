# Static glTF 2 binary import

`Import scene3dkit.GLTF` alongside `Import scene3dkit.Scene3DKit` exposes
`LoadGLB(filePath, triangleStride=1)` and `DecodeGLB(bytes, triangleStride=1)`.
Both return a `Mesh3D` containing the selected scene. Use the fully qualified
core import on case-insensitive filesystems (the BUI-513 workaround).

Node matrix or TRS transforms are baked into positions. Float32 `NORMAL`
accessors are transformed by the inverse transpose and normalized. Mirrored
transforms reverse triangle winding together with its normals and UVs. Missing
normals produce geometric face normals. `Triangle3D` retains `NormalA/B/C`,
`UV0A/B/C`, `UV1A/B/C` and `Material`; UVs are `Vec3D(u, v, 0)` values.

Geometry supports one embedded BIN buffer, float32 POSITION, unsigned
8/16/32-bit indices, interleaved vertex attributes, triangle primitives and
float32 or normalized unsigned 8/16-bit `TEXCOORD_0`/`TEXCOORD_1`. UVs can lie
outside the unit square for wrapping. Matrices must be affine. A mesh with
supplied normals rejects a singular node transform.

Materials retain the glTF metallic/roughness factors, linear base color,
emissive factor, alpha mode/cutoff and double-sided flag. Defaults follow
glTF: white base color, metallic and roughness 1, black emissive, opaque,
cutoff 0.5 and single-sided. The five core texture slots are supported:
base color, metallic/roughness, normal, occlusion and emissive. Each slot
selects UV set 0 or 1; the required attribute must be present. Normal scale
and occlusion strength are retained. Textures share decoded images while
preserving their sampler wrapping and minification/magnification filters.

Embedded `bufferView` PNG and JPEG images decode in memory into owned
`Picture` storage. `Texture3D.Image`, `Encoded`, `MimeType` and sampler
properties remain available to renderers. The separate
`buoy_scene3dimages` native library uses pinned stb_image 2.30, restricted to
PNG/JPEG with no filesystem access. MIME/signature, PNG framing/CRC, dimensions
and decoder allocation budgets are checked. Build its runtime slice with
`python3 tools/build_scene3dimages.py`; stage it under the package's runtime
directory before sealing. See `native/vendor/PROVENANCE.json` for its license
and pinned source. No image tooling process runs in the application.

The CPU renderer remains compatible through each triangle's base-color tint;
its flat shading does not reproduce the texture/PBR metadata. The GPU renderer
consumes the full material and vertex attributes. Import success alone does not
establish visual fidelity on a particular backend.

Sparse accessors, normalized POSITION/NORMAL, nontriangle modes, animation,
skinning, morph targets, required extensions, compressed primitives, texture
transforms, external buffers/images and cyclic/reused scene nodes reject.
Tangents, vertex colors and optional material extensions are not imported.
Normal maps use the GPU backend's UV derivative tangent frame. Materials must
use the core metallic/roughness workflow; extension-specific appearance is not
promised.

Maximum file size is 64 MiB, JSON chunk 1 MiB, JSON/node depth 64, node visits
4,096, aggregate decoded vertices/source triangles 1,048,576, and output
triangles 131,072. Referenced materials/textures have a 256-entry budget;
images have a 64-entry budget. Each encoded image is at most 16 MiB, each
side at most 4,096 pixels, and aggregate decoded images at most 16,777,216
pixels. The image decoder also bounds transient allocations. These are
explicit package limits, not glTF format limits. Nonfinite values, malformed
types/enums and invalid byte bounds/alignment reject. File reads cap size
before decoding. Embedded image bytes are copied before retention;
`DecodeGLB` does not retain the supplied GLB byte buffer after returning.

Triangle stride greater than one samples every Nth source triangle. It can
leave large holes and is intended for explicit diagnostics. For CPU visuals,
use properly simplified closed meshes or procedural primitives. Mind Games
includes reproducibly generated CPU meshes and preserved full originals;
the GPU path can use the full meshes and their smooth normals.

Verification commands:

Maintainers run the private validation suite before release. See the repository README for public build instructions.

These seal fresh packages and execute native consumers on the host. On this
Linux installation, set `LD_LIBRARY_PATH` to the existing JPEG8 compatibility
directory when the installed Buoy Picture runtime needs it. Compiler checks
on other targets are distinct from native renderer/image-decoder execution.
