---
summary: "Native GPU and CPU scenes, meshes, materials, orbit cameras, and bounded glTF import."
related: ["api.md", "getting-started.md", "typed-api.md"]
---

# Scene3DKit

Native GPU and CPU 3D scenes, meshes, materials, orbit cameras and bounded glTF.

Use qualified imports `scene3dkit.Scene3DKit`, `scene3dkit.GPU` and
`scene3dkit.GLTF` when combining modules. Add `MeshActor3D` instances to a
`Scene3D`; the shared `Camera3D` controls orbit, target, distance and perspective.
`LoadGLB` imports static glTF 2 geometry with smooth normals, UV0/UV1, material
factors and embedded PNG/JPEG textures. Primitive factories remain independently
usable for procedural scenes.

`GPUScene3D(width,height).Render(scene,camera)` draws with native shaders,
per-pixel depth, metallic/roughness materials, texture maps, studio reflections
and directional shadows, returning an owned Picture for Canvas composition.
Check `Renderer`, `Version` and `IsSoftware` for the actual backend. All context
operations stay on its owner thread; call `Close()` explicitly.

`scene.Draw(g,camera,width,height)` is the CPU fallback using native filled
triangle paths and painter ordering. It omits textures and advanced lighting.

See the package README and GLTF.md for ownership, supported features, bounds,
cache invalidation, native build dependencies, platform evidence and verification.
