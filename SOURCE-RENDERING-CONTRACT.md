# Mind Games source rendering contract

The native Buoy reimplementation consumes the sibling Rust project's authored
scenes. Source references below describe concrete parity targets, not a claim of
pixel identity between Bevy and Scene3DKit. The app normalizes source coordinates;
tests compare that documented transformation as well as actual raster effects.

## Camera effects

| Scene | Original reference | Required behavior |
| --- | --- | --- |
| Balloon | `../mind-games/src/balloon.rs:240–265` | HDR, 4x MSAA, ACES tone mapping, bloom intensity0.12, original diffuse/specular HDR maps |
| Candle/sparkler | `../mind-games/src/flame.rs:123–148` | HDR, 4x MSAA, ACES tone mapping, bloom intensity0.22, original HDR maps |

Requested samples and actual samples must be separate observations. Unsupported
4x multisample renderbuffers may fall back to1sample with an explicit capability
report; successful semantic checks alone never establish native MSAA support.
The local bloom is a bounded HDR bright extraction and separable Gaussian blur
composited before tone mapping, approximating Bevy Bloom::NATURAL rather than
claiming its exact multiscale kernel. Tests must prove glow outside emissive
geometry, controllable intensity, and multisampled edge coverage.

## Authored lights

The package supports8 point lights and1 spotlight, sufficient for these scenes.
Typed positions/colors/intensity/range are validated and submitted every frame.
The generic compatibility directional light is disabled in the application;
illumination and occlusion proofs must use the authored lights rather than masking
missing source effects with that extra light.
Original source colors are sRGB; PointLight3D/SpotLight3D Color channels are
linear-light values. The app converts source colors with the exact sRGB transfer
function before submission. Native attenuation uses bounded inverse-square falloff and range fade;
exact Bevy photometric constants are not claimed.

Balloon (`balloon.rs:392–453`):

- Five warm catenary point lights for `i=0,5,10,15,20`. Source position is
  `x=(i-10)*0.5`, `y=4.4+0.025*x*x`, `z=-2.6`; color(1,.68,.35), intensity35,
  range2.2, no authored shadows.
- Blue fill point at(3,3.5,-1.5), color(.55,.72,1), intensity16000, range14,
  no authored shadows.
- Warm spotlight at(-3,6,4), looking at(0,1.5,0), color(1,.88,.69),
  intensity60000, inner angle0.4, outer angle0.8 radians, shadows enabled.

Flame (`flame.rs:294–326`, `flame.rs:631–640`):

- Fire point at(.025,1.70,0), color(1,.56,.19), range5, shadows enabled.
  Its initial intensity350 becomes `(170+420*fireIntensity)*flicker` in candle
  mode or `(420+700*fireIntensity)*flicker` in sparkler mode.
  `flicker=1+.07*sin(14*t)+.035*sin(23.7*t)`.
- Blue fill point at(2,2.7,-2), color(.36,.49,.72), intensity380, range7,
  no authored shadows.
- Warm spotlight at(-2.5,4,3), looking at(0,1,0), color(1,.83,.58),
  intensity1600, inner angle0.45, outer angle0.85, shadows enabled.

Application coordinate mapping: balloon positions and ranges use uniform scale.7
with Y translation-1.55. Flame light positions use source Y translation-1.72;
the procedural wick/flame retains source units, yielding FireLight Y-.02.
Flame ranges and intensities retain source values. The imported candle body has
its existing independent bounding-box normalization; pixel identity of body/floor
placement with Bevy is not claimed.

Bevy spotlights omit explicit range in these declarations; the port chooses the
source default20 world units. One shadowing point light and one shadowing spot
are bounded reusable slots. The fire point uses six depth faces; the spotlight
uses its own perspective depth map. These shadows must move with their lights,
not merely reuse the compatibility directional shadow result. Light source radius
is retained as a visual-source distinction; this renderer does not claim area-light
soft-shadow identity with Bevy.

## Additive flame material prerequisite

`flame.rs:197–222` defines3 additive unlit layers with linear emissions
(3.8,.65,.045), (9,4.8,.85), (.65,1.8,5) and source alphas .36,.65,.6.
Scene3DKit already accumulates ADD in RGBA16F before a final tone pass, supports
unlit base color plus scaled HDR emission, and verifies exact sum/tone-map oracles.
The lighting/effects additions must retain those tests.

## Verification contract and entry points

`tests/scene3dkit_gpu_review.py` owns independent native pixel oracles and reports
`tests/scene3dkit_gpu_review_report.json`. It must test:

- Actual4sample edge coverage against1sample; report fallback capabilities.
- HDR bloom neighbors outside geometry, threshold/intensity response and disable.
- Nonemissive floor point-light response, distance/range/intensity/color changes.
- Spotlight cone acceptance/rejection and bounded per-slot light contributions.
- Separate point/spot occluder shadows, moving-light displacement, and disable flags.
- Existing depth, textures, PBR, HDR/additive/unlit, shadow acne, alpha, allocation,
  reload and owner-thread regressions.

`tests/scene3dkit_gpu_verify.py` compiles public GPU wrapper/consumer across6desktop
targets and executes fresh native Buoy consumer raster tests.
`tests/scene3dkit_gpu_native_verify.py` exercises the C ABI locally and optionally
on `--macos-host m4-vi`, retaining actual native backend/source hashes.
The app studio fidelity verifier must assert source settings and actual raster
contributions after normalization. A report is final only when tested source
hashes match unchanged files after execution. Native Windows fallback is reported
separately when the VM's driver lacks the required GL context.
