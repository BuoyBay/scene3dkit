# Render 3D scenes with meshes, materials and glTF assets

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

## Install

From a Buoy consumer project with BPM installed:

```sh
bpm add scene3dkit --git https://github.com/BuoyBay/scene3dkit.git --rev refs/heads/main
```

BPM records the resolved commit in its lockfile. Keep that lockfile with the
consumer project for reproducible installs.

## API entrypoints

- `Picture`
- `GPUScene3D(width,height,samples=4)`
- `Close()`
- `Samples`
- `Renderer`

## Build and API reference

Source is in `modules/`, with native/runtime inputs where required. Use the
[package guide](docs/API.md) for backend-specific build prerequisites, ownership
rules and supported targets. Rebuilding a native backend requires that target's
compiler and SDK; sealing an archive alone does not rebuild the backend.

## Local Buoy setup

On your own machine, install Buoy and run:

```sh
buoy ai init --both
```

The generated agent files and editor settings stay local and are ignored by Git.

## Contributing

[Contributing](CONTRIBUTING.md) · [License](LICENSE)
