# Scene3D rendering contract

## Camera and HDR effects

Requested and actual sample counts are separate observations. A requested 4x
multisample target may fall back to one sample when the backend cannot provide
it; callers must inspect capabilities. Bloom uses bounded HDR bright extraction
and separable Gaussian blur before tone mapping. Its threshold, intensity and
disable state should produce observable changes beyond emissive geometry.

## Lights and shadows

The renderer supports eight point lights and one spotlight. Positions, linear
colors, intensity and range are validated for submission. Attenuation uses
bounded inverse-square falloff with range fade; intensities do not promise a
separate engine's photometric calibration.

One point light and one spotlight have reusable shadow slots. Point shadows use
six depth faces; spot shadows use a perspective map. Shadow placement must follow
the selected light. Source radius does not promise area-light soft shadows.

## Additive materials

Additive unlit layers accumulate in RGBA16F before the final tone pass. Preserve
HDR emission, additive blending, depth, textures, alpha and owner-thread rules
when extending lighting or camera effects.

## Contributor verification

Exercise actual edge coverage, bloom outside geometry, light distance/range and
color response, spot-cone bounds, occluder displacement and disable flags.
Inspect native backend capabilities and rendered results after source changes;
compiler checking alone does not establish GPU behavior. Report Windows fallback
separately when the driver cannot provide the required GL context.
