# Handoff — Studio multi-amp + “Advanced” node editor

**Branch:** `feat/advanced-node-editor` · **Plugin version:** `2.6.28`  
**Files touched:** `screen.js`, `screen.html`, `plugin.json` (all frontend/plugin UI; `routes.py` was not touched).

> Reminder: for a **`screen.js`/CSS** change to reload in the app, you must **bump the `version` in `plugin.json`** (the app caches by `?v=version`). Changes to `screen.html` reload without a version bump. [...]

---

## 1. Studio room — multiple amps (parallel)

When the chain has 2–4 amps (for example, after adding one in parallel from Advanced), the room draws all of them, not just the first one.

- **Layout** (`RB_AMP_EXTRA_SLOTS` in `rbRenderStudioRoom`, `screen.js`): all amps are the **same size**, on the **same floor line** (`bottom:14%`, `width:168`, `translateZ:-140`, ig[...]
  - 2 amps → mirrored pair (primary `left:28%` rotateY(32) · mirrored `left:72%` rotateY(-32)).
  - 3 amps → plus one in the center (`left:50%`, straight).
  - 4 amps → plus two in the middle (`left:39%/61%`, straight, spaced apart).
- **Full 3D cabinets**: each amp = front + **side face** (`.rb-amp-stack::before`, mirrored to the right edge for amps facing left via `.rb-amp-extra::before`) + **top** ([...]
- **Critical GOTCHA:** **do not put `filter:` on `.rb-amp-extra`** — a `filter` on a `preserve-3d` element **flattens the 3D faces** (the side face disappeared and the amp looked like a flat photo[...]
- The contact shadow for each extra amp (`extraGroundHtml`) mirrors the primary one.

---

## 2. “Advanced” node editor

Graph editor (gear palette on the left + canvas with nodes and wires). It used to be visual-only; now it edits the real chain.

### Access
- **It is NO LONGER in the top bar** (next to Gear/Master). It opens from a floating **⚙ Advanced** button inside the Studio room (`.rb-studio-adv-btn`). Exit with the **Studio** button [...]

### Materializing gear (drag from the palette)
- `rbAdvMaterializeGear()`: dropping a gear item onto the canvas creates a **real piece** in the chain (`rbStudioCurrentChain` + `rbStudioPersist`), using the **catalog VST** (`vst_path/format/stat[...]
- The palette and the nodes use the **VST face** (`rbAdvGearImg`, via `RBPedalCanvas`), **never** RS photos (`/gear_photo/...`, scrubbed for DMCA reasons).

### Connect / disconnect
- **Connect:** drag from the output jack ● to another node (`rbAdvStartWire`/`rbAdvConnect`).
- **Disconnect:** **double-click** the wire (`dblclick` on `.rb-adv-cable-hit`). A single click does nothing (prevents accidental disconnects). There is no ✕ button and no red highlight.
- **Critical GOTCHA (root cause of “I can’t click the wire”):** the node layer (`#rb-adv-nodes`, z-index:2) was **above** the wire SVG and, even when empty, intercepted all pointer events. Fix:[...]

### Delete nodes
- **✕** button on hover for each node (`rbAdvDeleteNode`), or **drag the node outside the canvas** (it turns red; release = delete). Reconnects neighbors so the chain does not break, re-indexes [...]

### Bypass
- **Double-click a gear node** → `rbAdvToggleBypass`: the piece is bypassed (signal passes straight through like a wire), the node turns **gray (`saturate(0)`)**, it is persisted, and it also appears grayed out in the s[...]

### Pre/Post amp (two-way synchronization)
- **Advanced → chain** (`rbAdvSyncPedalSlots`, during connect/disconnect): a pedal’s pre/post slot is derived from the **graph topology** — if the pedal *feeds into* the amp = `pre_pedal`; if it [...]
- **Chain → Advanced** (`rbAdvRestore`): if the cached graph has a pedal in a position that **does not match** its current slot (because you changed it in the main UI), it discards the cache and **r[...]

### Zoom / pan
- **− / +** buttons (`rbAdvZoom`, 40–160%). **Pinch** (Mac gesture = `wheel` + `ctrlKey`) zooms. A **two-finger scroll pans** natively (does not zoom). *(Three-finger swipe cannot be captured by po[...]
- Wrapper `#rb-adv-zoom` keeps the correct scroll size while scaling; `rbAdvLayerPoint` subtracts the zoom so drag/drop/wire interactions remain accurate.

### Graph persistence
- The graph (positions + parallel wiring) is stored in **localStorage** per Studio view (`rbAdvStorageKey`: default/song/saved), so a parallel rig survives a restart. It is **visual-only**[...]

### Other
- The input node says **"Input"** (formerly "Guitar"); `rbAdvRestore` forces a fresh label so it does not remain cached.
- **Edit knobs from Advanced**: 🎛 button on node hover → floating card with the interactive VST canvas (`rbAdvEditNode`, reuses `rbStudioMakeFaceInteractive`); the knobs [...]

---

## 3. Audio (non-
