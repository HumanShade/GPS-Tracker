# Component: Power (Phase 3)

**Goal:** power the tracker reliably — a USB powerbank for prototyping, then the motorcycle's
12 V battery for unattended deployment, with a backup LiPo so it keeps reporting if main power is cut.

---

## Stage A — Prototype on a USB powerbank

**Tasks**
1. Power the board from a standard USB powerbank for bench/field testing.
2. Measure runtime in your actual duty cycle (sleep + wake-on-motion).

**⚠️ The big gotcha: powerbank auto-shutoff.**
A deep-sleeping ESP32 draws so little current that **many powerbanks decide nothing is plugged in
and switch themselves off.** Mitigations:
- Use a powerbank known to **not auto-shutoff** (some "always-on" / low-current modes exist).
- Or keep average draw above the powerbank's cutoff threshold (defeats low-power, only for early testing).
- Or skip straight to a **LiPo** on the board's battery input for realistic low-power testing — the
  LilyGO board charges/runs from LiPo directly and won't have the USB cutoff behavior.

**Rough battery expectations (from planning discussion, ~10 000 mAh powerbank ≈ 6000–6500 mAh usable at 5 V):**

| Mode | Avg current | Ballpark runtime |
|---|---|---|
| Deep sleep + motion-triggered reporting | ~15 mA | ~2+ weeks |
| Duty-cycled tracking (Cat-1 is thirstier than LTE-M) | ~60 mA+ | a few days |
| Continuous streaming over LTE | ~200 mA | ~1 day |

> Cat-1 draws more than the LTE-M numbers quoted earlier, so favor **wake-on-motion + sleep**.
> For weeks-unattended, the real answer is Stage B (bike power), not a bigger powerbank.

**Acceptance**
- The device runs untethered through several sleep→motion→report→sleep cycles without the supply
  cutting out.

---

## Stage B — Motorcycle 12 V battery (production)

**Goal:** survive **weeks parked in public** on bike power, with LiPo backup.

**Tasks**
1. Tap the bike's **12 V** through an **inline fuse** (low value, e.g. 1–2 A) at the take-off point.
2. Step down with a **buck converter rated for the automotive environment** (handles ~12–14.4 V
   charging voltage, tolerant of transients/load-dump; add reverse-polarity + TVS protection if the
   converter doesn't include it). Output to **5 V/USB** or the board's V_in.
3. Connect a **backup LiPo** to the board's battery input (LilyGO board charges it). If main power
   is cut, the LiPo keeps Layer 2 reporting.
4. **Measure parasitic draw** with a multimeter. Confirm the always-on draw is low enough that it
   **won't flatten the bike battery** over weeks of parking. Wake-on-motion sleep is what makes this OK.
5. Decide tap point: **switched** (only powered with ignition — safer for the bike battery, but no
   tracking while off → bad for theft) vs **always-on/battery** (tracks while parked — what you want,
   so parasitic draw discipline is essential). Choose **always-on** and keep sleep current tiny.

**Acceptance**
- Bench-simulate 12 V (bench supply), confirm correct 5 V out and stable operation.
- On the bike: measured parasitic draw is low enough for weeks of parking; cutting main power, the
  device still reports from LiPo.

**Safety / correctness notes**
- **Fuse the tap** close to the battery — non-negotiable on vehicle wiring.
- Confirm the fuse/parasitic approach **for this specific motorcycle** (charging system varies).
- Mind **load dump / voltage spikes** in automotive electrical systems — protect the input.
- Mount the electronics in a **non-metal enclosure** with the GPS antenna having sky view and the
  LTE antenna clear of large metal masses; hidden under the seat is fine if antennas are placed well.

---

## How power ties to the rest of the design
- **Wake-on-motion (LIS3DH)** is what lets bike power work without draining the battery — radios off
  while parked, on only when moving. See [`component-cellular-gps.md`](component-cellular-gps.md) 2c.
- The **BLE crowdsourced layer** sips almost nothing and can keep advertising even in low-power states.
- Size the power budget around the **cellular layer** (the expensive one), not BLE.

## Open questions
- Exact buck converter model with automotive-grade input protection.
- LiPo capacity (how long must it survive after main power is cut?).
- Final tap point + fuse rating for the specific bike.
