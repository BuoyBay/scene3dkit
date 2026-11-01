---
title: "scene3dkit Getting started"
summary: "Install, import, and explore the public scene3dkit API."
category: "Guide"
symbols: ["GPUScalar3D", "GPUIndex3D", "GPUVertex3D", "GPUScene3D", "Vec3D", "Texture3D", "Material3D", "Triangle3D", "Mesh3D", "MeshActor3D", "Camera3D", "Scene3D"]
related: ["index.md", "api.md", "typed-api.md"]
---

# Getting started

Install the scene3dkit package into your project’s packages directory. Import the module you need: `GLTF`, `GLTFImages`, `GPU`, `Scene3DKit`.

This archive includes x86_64 Linux and Windows native libraries. Windows is
cross-built; GPU execution was not validated for either packaged slice. macOS
and ARM native libraries must be built from matching sources before using the
GPU renderer or image decoder on those targets.

Start with the [API contracts](api.md) for usage, constraints, and platform limitations. The [typed API](typed-api.md) lists constructors, each parameter and its default, properties, and return values directly from source.

[Package overview](index.md)
