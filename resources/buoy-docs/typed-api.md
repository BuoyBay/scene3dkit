---
title: "scene3dkit typed API"
summary: "Source-derived public signatures, properties, parameters, defaults, and return values."
category: "Reference"
order: 21
types: ["Camera3D", "GPUIndex3D", "GPUScalar3D", "GPUScene3D", "GPUVertex3D", "Material3D", "Mesh3D", "MeshActor3D", "PointLight3D", "Scene3D", "SpotLight3D", "Texture3D", "Triangle3D", "Vec3D"]
dependencies: []
symbols: ["Camera3D", "GPUIndex3D", "GPUScalar3D", "GPUScene3D", "GPUVertex3D", "Material3D", "Mesh3D", "MeshActor3D", "PointLight3D", "Scene3D", "SpotLight3D", "Texture3D", "Triangle3D", "Vec3D"]
related: ["api.md", "getting-started.md", "index.md"]
---

# scene3dkit typed API

Generated from the package source declarations. Platform guards are shown where present; inherited members are documented on the base type. See the [API contracts](api.md) for behavior and limitations.

## GLTF functions

### DecodeGLB

`Public Function DecodeGLB(data As Bytes, triangleStride As Integer = 1) As Mesh3D`

Decode a bounded static glTF 2 binary scene. Node transforms are baked. Stride >1 explicitly samples every Nth source triangle; it is not mesh simplification.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `data` | `Bytes` | `ByVal` | Required |
| `triangleStride` | `Integer` | `ByVal` | `1` |

#### Returns

| Type |
| --- |
| `Mesh3D` |

### LoadGLB

`Public Function LoadGLB(filePath As String, triangleStride As Integer = 1) As Mesh3D`

Load an embedded-buffer GLB file. File size is capped before allocation.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `filePath` | `String` | `ByVal` | Required |
| `triangleStride` | `Integer` | `ByVal` | `1` |

#### Returns

| Type |
| --- |
| `Mesh3D` |

## GLTFImages functions

### DecodeGLTFImage

`Public Function DecodeGLTFImage(data As Bytes, mimeType As String, maxPixels As Integer = 16777216) As Picture`

Decode embedded GLB PNG/JPEG bytes into owned straight-RGBA Picture storage. No file is created. MIME/signature, framing and dimensions are validated before allocation.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `data` | `Bytes` | `ByVal` | Required |
| `mimeType` | `String` | `ByVal` | Required |
| `maxPixels` | `Integer` | `ByVal` | `16777216` |

#### Returns

| Type |
| --- |
| `Picture` |

## GPUScalar3D

`Sequential Structure GPUScalar3D`

### Fields

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `Value` | `Double` | — | — |

## GPUIndex3D

`Sequential Structure GPUIndex3D`

### Fields

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `Value` | `Int32` | — | — |

## GPUVertex3D

`Sequential Structure GPUVertex3D`

### Fields

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `X` | `Double` | — | — |
| `Y` | `Double` | — | — |
| `Z` | `Double` | — | — |
| `NX` | `Double` | — | — |
| `NY` | `Double` | — | — |
| `NZ` | `Double` | — | — |
| `U` | `Double` | — | — |
| `V` | `Double` | — | — |
| `U1` | `Double` | — | — |
| `V1` | `Double` | — | — |
| `R` | `Double` | — | — |
| `G` | `Double` | — | — |
| `B` | `Double` | — | — |
| `A` | `Double` | — | — |

## GPUScene3D

`Public Class GPUScene3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `ClearColor` | `Color` | No | No | `Color.RGBA(0.0,0.0,0.0,0.0)` | — |
| `Exposure` | `Double` | No | No | `1.0` | — |
| `Shadows` | `Boolean` | No | No | `True` | — |
| `DirectionalLightEnabled` | `Boolean` | No | No | `True` | — |
| `BloomStrength` | `Double` | No | No | `0.0` | — |
| `PointLight` | `Vec3D` | No | No | — | — |
| `PointColor` | `Color` | No | No | `Color.White` | — |
| `PointIntensity` | `Double` | No | No | `0.0` | — |
| `PointRange` | `Double` | No | No | `10.0` | — |
| `PointLights` | `Array Of PointLight3D` | No | No | `[]` | — |
| `SpotLight` | `SpotLight3D` | No | No | — | — |
| `Samples` | `Integer` | Yes | No | — | — |
| `Width` | `Integer` | Yes | No | — | — |
| `Height` | `Integer` | Yes | No | — | — |
| `Renderer` | `String` | Yes | No | — | — |
| `Version` | `String` | Yes | No | — | — |
| `IsSoftware` | `Boolean` | Yes | No | — | — |
| `CachedMeshes` | `Integer` | Yes | No | — | — |
| `CachedTextures` | `Integer` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(width As Integer, height As Integer, samples As Integer = 4)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `width` | `Integer` | `ByVal` | Required |
| `height` | `Integer` | `ByVal` | Required |
| `samples` | `Integer` | `ByVal` | `4` |

Returns: no value.

### SetEnvironment

`Public Sub SetEnvironment(diffuse As Bytes, specular As Bytes)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `diffuse` | `Bytes` | `ByVal` | Required |
| `specular` | `Bytes` | `ByVal` | Required |

Returns: no value.

### ClearCache

`Public Sub ClearCache()`

No parameters.

Returns: no value.

### InvalidateMesh

`Public Sub InvalidateMesh(mesh As Mesh3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `mesh` | `Mesh3D` | `ByVal` | Required |

Returns: no value.

### InvalidateTexture

`Public Sub InvalidateTexture(texture As Texture3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `texture` | `Texture3D` | `ByVal` | Required |

Returns: no value.

### Render

`Public Function Render(scene As Scene3D, camera As Camera3D) As Picture`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `scene` | `Scene3D` | `ByVal` | Required |
| `camera` | `Camera3D` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Picture` |

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

## Vec3D

`Public Class Vec3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `X` | `Double` | Yes | No | — | — |
| `Y` | `Double` | Yes | No | — | — |
| `Z` | `Double` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(x As Double, y As Double, z As Double)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `x` | `Double` | `ByVal` | Required |
| `y` | `Double` | `ByVal` | Required |
| `z` | `Double` | `ByVal` | Required |

Returns: no value.

## PointLight3D

`Public Class PointLight3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `CastShadows` | `Boolean` | No | No | `False` | — |
| `Position` | `Vec3D` | No | No | `New Vec3D(0.0, 0.0, 0.0)` | — |
| `Color` | `Color` | No | No | `Color.White` | — |
| `Intensity` | `Double` | No | No | `0.0` | — |
| `Range` | `Double` | No | No | `10.0` | — |

### Validate

`Public Sub Validate()`

No parameters.

Returns: no value.

## SpotLight3D

`Public Class SpotLight3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `CastShadows` | `Boolean` | No | No | `False` | — |
| `Position` | `Vec3D` | No | No | `New Vec3D(0.0, 0.0, 0.0)` | — |
| `Direction` | `Vec3D` | No | No | `New Vec3D(0.0, -1.0, 0.0)` | — |
| `Color` | `Color` | No | No | `Color.White` | — |
| `Intensity` | `Double` | No | No | `0.0` | — |
| `Range` | `Double` | No | No | `20.0` | — |
| `InnerAngle` | `Double` | No | No | `0.4` | — |
| `OuterAngle` | `Double` | No | No | `0.8` | — |

### Validate

`Public Sub Validate()`

No parameters.

Returns: no value.

## Texture3D

`Public Class Texture3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Image` | `Picture` | No | No | — | — |
| `Encoded` | `Bytes` | No | No | — | — |
| `MimeType` | `String` | No | No | `"image/png"` | — |
| `WrapS` | `Integer` | No | No | `10497` | — |
| `WrapT` | `Integer` | No | No | `10497` | — |
| `MinFilter` | `Integer` | No | No | `9987` | — |
| `MagFilter` | `Integer` | No | No | `9729` | — |

### Constructor

`Public Sub Constructor(image As Picture)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `image` | `Picture` | `ByVal` | Required |

Returns: no value.

## Material3D

`Public Class Material3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `BaseColor` | `Color` | No | No | `Color.White` | — |
| `Metallic` | `Double` | No | No | `1.0` | — |
| `Roughness` | `Double` | No | No | `1.0` | — |
| `Emissive` | `Color` | No | No | `Color.Black` | — |
| `EmissiveStrength` | `Double` | No | No | `1.0` | — |
| `Unlit` | `Boolean` | No | No | `False` | — |
| `NormalScale` | `Double` | No | No | `1.0` | — |
| `OcclusionStrength` | `Double` | No | No | `1.0` | — |
| `AlphaMode` | `String` | No | No | `"OPAQUE"` | — |
| `AlphaCutoff` | `Double` | No | No | `0.5` | — |
| `DoubleSided` | `Boolean` | No | No | `False` | — |
| `BaseColorTexture` | `Texture3D` | No | No | — | — |
| `BaseColorTexCoord` | `Integer` | No | No | `0` | — |
| `MetallicRoughnessTexture` | `Texture3D` | No | No | — | — |
| `MetallicRoughnessTexCoord` | `Integer` | No | No | `0` | — |
| `NormalTexture` | `Texture3D` | No | No | — | — |
| `NormalTexCoord` | `Integer` | No | No | `0` | — |
| `OcclusionTexture` | `Texture3D` | No | No | — | — |
| `OcclusionTexCoord` | `Integer` | No | No | `0` | — |
| `EmissiveTexture` | `Texture3D` | No | No | — | — |
| `EmissiveTexCoord` | `Integer` | No | No | `0` | — |

### Validate

`Public Sub Validate()`

No parameters.

Returns: no value.

## Triangle3D

`Public Class Triangle3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `NormalA` | `Vec3D` | No | No | — | — |
| `NormalB` | `Vec3D` | No | No | — | — |
| `NormalC` | `Vec3D` | No | No | — | — |
| `UV0A` | `Vec3D` | No | No | — | — |
| `UV0B` | `Vec3D` | No | No | — | — |
| `UV0C` | `Vec3D` | No | No | — | — |
| `UV1A` | `Vec3D` | No | No | — | — |
| `UV1B` | `Vec3D` | No | No | — | — |
| `UV1C` | `Vec3D` | No | No | — | — |
| `Material` | `Material3D` | No | No | — | — |
| `A` | `Vec3D` | Yes | No | — | — |
| `B` | `Vec3D` | Yes | No | — | — |
| `C` | `Vec3D` | Yes | No | — | — |
| `Tint` | `Color` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(a As Vec3D, b As Vec3D, c As Vec3D, tint As Color)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `a` | `Vec3D` | `ByVal` | Required |
| `b` | `Vec3D` | `ByVal` | Required |
| `c` | `Vec3D` | `ByVal` | Required |
| `tint` | `Color` | `ByVal` | Required |

Returns: no value.

## Mesh3D

`Public Class Mesh3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Count` | `Integer` | Yes | No | — | — |

### AddTriangle

`Public Sub AddTriangle(a As Vec3D, b As Vec3D, c As Vec3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `a` | `Vec3D` | `ByVal` | Required |
| `b` | `Vec3D` | `ByVal` | Required |
| `c` | `Vec3D` | `ByVal` | Required |

Returns: no value.

### AddTriangle (overload 2)

`Public Sub AddTriangle(a As Vec3D, b As Vec3D, c As Vec3D, tint As Color)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `a` | `Vec3D` | `ByVal` | Required |
| `b` | `Vec3D` | `ByVal` | Required |
| `c` | `Vec3D` | `ByVal` | Required |
| `tint` | `Color` | `ByVal` | Required |

Returns: no value.

### AddSurfaceTriangle

`Public Sub AddSurfaceTriangle(a As Vec3D, b As Vec3D, c As Vec3D, na As Vec3D, nb As Vec3D, nc As Vec3D, uv0a As Vec3D, uv0b As Vec3D, uv0c As Vec3D, uv1a As Vec3D, uv1b As Vec3D, uv1c As Vec3D, material As Material3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `a` | `Vec3D` | `ByVal` | Required |
| `b` | `Vec3D` | `ByVal` | Required |
| `c` | `Vec3D` | `ByVal` | Required |
| `na` | `Vec3D` | `ByVal` | Required |
| `nb` | `Vec3D` | `ByVal` | Required |
| `nc` | `Vec3D` | `ByVal` | Required |
| `uv0a` | `Vec3D` | `ByVal` | Required |
| `uv0b` | `Vec3D` | `ByVal` | Required |
| `uv0c` | `Vec3D` | `ByVal` | Required |
| `uv1a` | `Vec3D` | `ByVal` | Required |
| `uv1b` | `Vec3D` | `ByVal` | Required |
| `uv1c` | `Vec3D` | `ByVal` | Required |
| `material` | `Material3D` | `ByVal` | Required |

Returns: no value.

### Triangles

`Public Function Triangles() As Array Of Triangle3D`

No parameters.

#### Returns

| Type |
| --- |
| `Array Of Triangle3D` |

## MeshActor3D

`Public Class MeshActor3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Tint` | `Color` | No | No | — | — |
| `X` | `Double` | No | No | `0.0` | — |
| `Y` | `Double` | No | No | `0.0` | — |
| `Z` | `Double` | No | No | `0.0` | — |
| `ScaleX` | `Double` | No | No | `1.0` | — |
| `ScaleY` | `Double` | No | No | `1.0` | — |
| `ScaleZ` | `Double` | No | No | `1.0` | — |
| `RotationY` | `Double` | No | No | `0.0` | — |
| `Visible` | `Boolean` | No | No | `True` | — |
| `Metallic` | `Double` | No | No | `0.3` | — |
| `Roughness` | `Double` | No | No | `0.28` | — |
| `MaterialOverride` | `Material3D` | No | No | — | — |

### Constructor

`Public Sub Constructor(mesh As Mesh3D, tint As Color)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `mesh` | `Mesh3D` | `ByVal` | Required |
| `tint` | `Color` | `ByVal` | Required |

Returns: no value.

### Mesh

`Public Function Mesh() As Mesh3D`

No parameters.

#### Returns

| Type |
| --- |
| `Mesh3D` |

### SetMesh

`Public Sub SetMesh(mesh As Mesh3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `mesh` | `Mesh3D` | `ByVal` | Required |

Returns: no value.

### Validate

`Public Sub Validate()`

No parameters.

Returns: no value.

### Transform

`Public Function Transform(point As Vec3D) As Vec3D`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `point` | `Vec3D` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Vec3D` |

## Camera3D

`Public Class Camera3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `Distance` | `Double` | No | No | `6.0` | — |
| `TargetY` | `Double` | No | No | `0.0` | — |
| `FocalLength` | `Double` | No | No | `0.95` | — |
| `NearPlane` | `Double` | No | No | `0.05` | — |
| `Yaw` | `Double` | Yes | No | — | — |
| `Pitch` | `Double` | Yes | No | — | — |

### Orbit

`Public Sub Orbit(yawRadians As Double, pitchRadians As Double)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `yawRadians` | `Double` | `ByVal` | Required |
| `pitchRadians` | `Double` | `ByVal` | Required |

Returns: no value.

### Validate

`Public Sub Validate()`

No parameters.

Returns: no value.

### View

`Public Function View(point As Vec3D) As Vec3D`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `point` | `Vec3D` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Vec3D` |

## Scene3D

`Public Class Scene3D`

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `LastDrawnFaces` | `Integer` | Yes | No | — | — |
| `Count` | `Integer` | Yes | No | — | — |

### Actors

`Public Function Actors() As Array Of MeshActor3D`

No parameters.

#### Returns

| Type |
| --- |
| `Array Of MeshActor3D` |

### Add

`Public Sub Add(actor As MeshActor3D)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `actor` | `MeshActor3D` | `ByVal` | Required |

Returns: no value.

### Remove

`Public Function Remove(actor As MeshActor3D) As Boolean`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `actor` | `MeshActor3D` | `ByVal` | Required |

#### Returns

| Type |
| --- |
| `Boolean` |

### Clear

`Public Sub Clear()`

No parameters.

Returns: no value.

### Draw

`Public Sub Draw(g As IGraphics, camera As Camera3D, width As Double, height As Double)`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `g` | `IGraphics` | `ByVal` | Required |
| `camera` | `Camera3D` | `ByVal` | Required |
| `width` | `Double` | `ByVal` | Required |
| `height` | `Double` | `ByVal` | Required |

Returns: no value.

## Scene3DKit functions

### BalloonMesh3D

`Public Function BalloonMesh3D(shape As Integer = 0, segments As Integer = 32, rings As Integer = 20) As Mesh3D`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `shape` | `Integer` | `ByVal` | `0` |
| `segments` | `Integer` | `ByVal` | `32` |
| `rings` | `Integer` | `ByVal` | `20` |

#### Returns

| Type |
| --- |
| `Mesh3D` |

### SphereMesh3D

`Public Function SphereMesh3D(segments As Integer = 24, rings As Integer = 16) As Mesh3D`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `segments` | `Integer` | `ByVal` | `24` |
| `rings` | `Integer` | `ByVal` | `16` |

#### Returns

| Type |
| --- |
| `Mesh3D` |

### CylinderMesh3D

`Public Function CylinderMesh3D(segments As Integer = 24) As Mesh3D`

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `segments` | `Integer` | `ByVal` | `24` |

#### Returns

| Type |
| --- |
| `Mesh3D` |
