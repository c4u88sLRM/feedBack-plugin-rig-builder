# Bass Amp Rework Handoff

Date: 2026-06-22  
Expected branch: `feat/amps-rework`

This document is for whoever continues the bass amp work. The idea is to reuse
the new real-amp framework without copying GPL code from Guitarix or having to
rebuild tables/tube models from scratch.

## What already exists

Base files:

- `vst/src/_shared/tube_stage.hpp`  
  Reusable physical framework: `TubeStageT`, `PowerAmpPPT`, `ToneStackYeh`,
  `TweedTone`, `CouplingCapGridLeak`, `PhaseInverterLTP12AX7`,
  `PhaseInverterCathodyne12AX7`, `MultiNodeBPlus`, `PotTaper`.
- `vst/src/_shared/koren*_ftube.h`  
  Generated Koren tables for 12AX7, 12AY7, EF86, EL84, 6V6, 5881, 6L6GC,
  KT66, and EL34.
- `../tubes/*.pdf` in the local root of `slopsmith`  
  Datasheets used for Miller capacitances and table points. Note that this
  folder is outside the `rig_builder` Git repo, so it will not be included by
  `git push origin feat/amps-rework` unless it is copied into `rig_builder/`
  or shared by some other means. The `koren*_ftube.h` headers do remain inside
  `rig_builder`.
- `vst/src/amps/tools/gx_tube.py`  
  Generator/auditor for Koren tables.
- `vst/src/amps/tools/calibrate_amp_core.py`  
  Offline harness for stability, gain sweep, crest factor, approximate THD,
  and spectral checkpoints. Right now it includes a spec for `en30`; add bass
  amp specs there.
- `vst/src/amps/REAL_TUBE_AMP_GUIDE.md`  
  Complete technical guide for the new workflow.
- `vst/src/amps/en30/BoxDC30Core.h`  
  Advanced pilot example: BOX AC30 with Miller effect, blocking distortion,
  LTP, multi-node GZ34 supply, pot tapers, and reactive output.

Existing bass examples to audit or port:

- `vst/src/amps/sampleg_sbtcl/` - SVT-CL style.
- `vst/src/amps/cs75b_v4b/` - Ampeg V-4B style.

Do not assume those two are already “finished” just because they compile:
review schematic, topology, knobs, loudness, crest, and live testing.

## Recommended workflow for a bass amp

1. Identify the real amp and get a readable schematic.
2. Make a component-by-component inventory:
   - preamp and power tubes
   - coupling caps and grid leaks
   - cathode resistors / bypass caps
   - tone stack and switches
   - phase inverter
   - rectifier / supply
   - NFB, presence / deep / ultra-low / ultra-high
   - power amp and expected load / speaker
3. If a new tube appears:
   - add the PDF to `tubes/`
   - add constants in `vst/src/amps/tools/gx_tube.py`
   - generate `vst/src/_shared/koren_<tube>_ftube.h`
   - add the trait in `tube_stage.hpp`
4. Choose blocks:
   - Preamp: `TubeStageT<Tube...>`
   - Miller: `MillerLowPassT<Tube...>`
   - Interstage caps: `CouplingCapGridLeak`
   - Tone stack: `ToneStackYeh` or a custom block if the circuit is not a
     standard TMB
   - LTP PI: `PhaseInverterLTP12AX7`
   - Cathodyne PI: `PhaseInverterCathodyne12AX7`
   - Supply: `MultiNodeBPlus` with the amp’s own profile
   - Power: `PowerAmpPPT<Tube...>`
   - Knobs: `PotTaper::audio`, `reverseAudio`, `sCurve`, `switchBlend`
5. Map Rocksmith controls to real controls. Do not map linearly by default:
   real pots are often audio/log, reverse log, or switch-like.
6. Add a spec in `calibrate_amp_core.py` for the amp.
7. Validate:
   - stability at 48 / 96 / 192 kHz with no NaN/inf
   - gain sweep with coherent crest factor and THD
   - usable RMS/loudness
   - spectral checkpoints in lows and mids
   - live testing inside feedBack

## Bass-specific considerations

- Low end is king. Do not use aggressive high-pass filters from guitar designs.
  Check the real coupling caps: many bass amps use large values specifically to
  avoid thinning out the signal.
- The power amp and output transformer must have believable headroom and
  low-frequency behavior. Bass can excite sag and blocking differently from
  guitar.
- Ultra Low / Ultra High / Deep / Bright type switches are not generic EQ:
  they usually alter specific caps/resistors around the tone stack or feedback
  loop.
- If the real amp has strong NFB, model it. In bass amps, damping changes the
  attack and the sense of tightness a lot.
- Validate with bass DI, not only with `ui_public_inputs_Brit - Guitar.wav`.
  For the harness, add an input or excitation with bass fundamentals
  (approx. 41, 55, 73, 82, 110, 220, 440 Hz).
- Separate amp and cab. The amp should not bake in the entire cab curve if
  feedBack will apply an IR afterward; but if a reactive load is modeled, it
  should represent what the power amp “sees,” not a decorative final EQ.

## Implementation checklist

- Create or audit `vst/src/amps/<amp>/`.
- Use oversampling in the wrapper if there are strong nonlinearities.
- Keep the core offline-testable, ideally in `<Amp>Core.h`.
- Avoid generic `tanh` as a tube substitute when a schematic exists.
- Use `ToneStackYeh` with real values where applicable.
- Use `CouplingCapGridLeak` where a grid can conduct and charge the cap.
- Use the correct PI for the topology; do not go straight into
  `PowerAmpPPT` if the amp has a relevant phase inverter.
- Use `MultiNodeBPlus` or create a supply profile specific to the amp.
- Document the amp’s status and remaining testing in `REAL_TUBE_AMP_GUIDE.md`.
- Rebuild, install bundle, sign, and run the harness.

## Useful commands

Build an amp:

```bash
make -C rig_builder/vst/src/amps/<amp_dir> BASE_PATH=/Users/nacho/Files/slopsmith/rig_builder/vst/src/amps/<amp_dir>
```

Install the macOS binary into the bundle:

```bash
install -m 755 rig_builder/vst/src/amps/<amp_dir>/bin/<Plugin>.vst3/Contents/MacOS/<Plugin> "rig_builder/vst/amps/<Bundle Name>.vst3/Contents/MacOS/<Plugin>"
codesign --force -s - "rig_builder/vst/amps/<Bundle Name>.vst3"
codesign --verify --deep --strict --verbose=2 "rig_builder/vst/amps/<Bundle Name>.vst3"
```

Validate AC30 as an example:

```bash
python3 rig_builder/vst/src/amps/tools/calibrate_amp_core.py en30
python3 rig_builder/vst/src/amps/tools/loudness_check.py
```

## What to push to the branch to share

The current local branch is `feat/amps-rework`. For another developer to reuse
this, at minimum the following must reach the remote:

- `vst/src/_shared/tube_stage.hpp`
- `vst/src/_shared/koren*_ftube.h`
- `vst/src/amps/tools/gx_tube.py`
- `vst/src/amps/tools/calibrate_amp_core.py`
- `vst/src/amps/REAL_TUBE_AMP_GUIDE.md`
- `docs/BASS_AMP_REWORK_HANDOFF.md`
- any cores / params / plugins intended to be shared as references
- PDFs from `tubes/*.pdf` via one of these options:
  - copy them into `rig_builder/tubes/` and commit them there, or
  - share the `tubes/` folder outside Git, or
  - convert them into a separate repo/submodule if more will keep being added.

Do not run `git add -A` in this worktree without reviewing it first: there are
currently many modified macOS bundles, deleted Windows/Linux payloads, and
untracked `bin/` directories. For a clean push, stage only what is needed.

Example of a selective push:

```bash
git -C rig_builder status --short --branch
git -C rig_builder add docs/BASS_AMP_REWORK_HANDOFF.md \
  vst/src/_shared/tube_stage.hpp \
  vst/src/_shared/koren5881_ftube.h \
  vst/src/_shared/koren6l6gc_ftube.h \
  vst/src/_shared/koren_ef86_ftube.h \
  vst/src/_shared/koren_kt66_ftube.h \
  vst/src/amps/tools/calibrate_amp_core.py \
  vst/src/amps/tools/gx_tube.py \
  vst/src/amps/REAL_TUBE_AMP_GUIDE.md
git -C rig_builder diff --cached --check
git -C rig_builder commit -m "Document amp rework framework for bass ports"
git -C rig_builder push origin feat/amps-rework
```

If you also want to share the BOX AC30 pilot, add its mapping/core/bundle files
explicitly, reviewing the diff before committing.

If you also want to upload the PDFs on the same branch, copy them first:

```bash
mkdir -p rig_builder/tubes
cp tubes/*.pdf rig_builder/tubes/
git -C rig_builder add tubes/*.pdf
```
