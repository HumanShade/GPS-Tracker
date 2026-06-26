# Architecture

## Overview

The tracker is a single ESP32-S3-based device that reports its location through two
**independent, parallel channels**. Neither depends on the other; either alone is useful.

```
                          ┌─────────────────────────────────────────────┐
                          │              TRACKER DEVICE                  │
                          │              (ESP32-S3)                      │
                          │                                              │
   ┌──────────────┐       │   ┌───────────────────────────────────┐     │
   │ Accelerometer│──IRQ──┼──▶│  Firmware state machine           │     │
   │  (LIS3DH)    │ wake  │   │  sleep → wake-on-motion → act     │     │
   └──────────────┘       │   └───────────────┬───────────────────┘     │
                          │                   │                         │
                          │        ┌──────────┴───────────┐             │
                          │        ▼                      ▼             │
                          │  ┌────────────┐        ┌─────────────┐      │
                          │  │ BLE radio  │        │ Cellular +  │      │
                          │  │ (beacons)  │        │ GPS modem   │      │
                          │  └─────┬──────┘        └──────┬──────┘      │
                          └────────┼──────────────────────┼────────────┘
                                   │                       │
              LAYER 1: CROWDSOURCED (free, passive)        │  LAYER 2: CELLULAR (real-time)
                                   │                       │
            ┌──────────────────────┴───────┐               │
            ▼                              ▼               ▼
   ┌─────────────────┐          ┌──────────────────┐   ┌──────────────────┐
   │ Nearby iPhones  │          │ Nearby Androids  │   │ Cell network     │
   │ (Apple Find My) │          │ (Google FMDN)    │   │ (LTE Cat-1)      │
   └────────┬────────┘          └────────┬─────────┘   └────────┬─────────┘
            ▼                            ▼                      │
   ┌─────────────────┐          ┌──────────────────┐           ├──▶ SMS w/ maps link → my phone
   │ Apple servers   │          │ Google servers   │           │
   └────────┬────────┘          └────────┬─────────┘           └──▶ (optional) data → Traccar
            ▼                            ▼                              → browser/app map
   ┌─────────────────┐          ┌──────────────────┐
   │ Macless-Haystack│          │ GoogleFindMyTools│
   │ viewer (mine)   │          │ script (mine)    │
   └─────────────────┘          └──────────────────┘
```

## Layer 1 — Crowdsourced BLE

**Principle:** the device only *broadcasts* BLE advertisements. A stranger's phone passing
nearby hears it and uploads the location to Apple/Google **over that stranger's data plan**.
I read the result back through the project tools over my home internet.

- **No SIM, no subscription, near-zero power.** Runs even in deep-sleep-ish states.
- **Two sub-networks, two packet formats**, time-multiplexed on one BLE radio:
  - **Apple Find My** — via Macless-Haystack. Mature. Precomputes weeks of rolling keys → fit-and-forget.
  - **Google Find My Device** — via GoogleFindMyTools ESP32 firmware. Newer; needs ~3-day re-registration; no MAC randomization yet.
- **Coverage** depends on foot traffic. Great in a public garage / street parking; dead in a sealed underground level or remote area.
- **Latency/accuracy:** minutes-to-hours stale, ~10 m, no way to force an update.
- **Role:** free, best-effort **secondary**. "Where it *was* when last seen."

## Layer 2 — Cellular + GPS

**Principle:** the device gets its own GPS fix and pushes it over its own SIM. Fully under my control.

- **LTE Cat-1 (SIM7670G)** — broad coverage, no LTE-M/NB-IoT region check.
- **GNSS** for the actual fix; cellular for the uplink.
- **Reporting paths:**
  - **SMS + Google Maps link** (`https://maps.google.com/?q=lat,lon`) — easiest, zero backend, great as instant alert.
  - **Data → Traccar** (optional, later) — live browser/app map, history, geofencing.
- **Latency/accuracy:** seconds, GPS-grade, on demand.
- **Role:** reliable **primary**. "Where it *is*, right now."

## The trigger: wake-on-motion

A **LIS3DH accelerometer** with a hardware interrupt is the hinge that makes the whole thing
practical:

```
deep sleep  ──(motion IRQ)──▶  wake  ──▶  GPS fix  ──▶  report (SMS / data)  ──▶  back to sleep
```

This gives both **long battery life** (radios off while parked) and an **instant theft alert**
(movement → wake → report within seconds). A parked bike's location doesn't change, so there's
nothing to report until it moves.

## Power architecture

| Stage | Source | Notes |
|---|---|---|
| Prototype | USB powerbank | Watch the **low-draw auto-shutoff** gotcha (see component-power.md) |
| Production | Bike 12 V → buck converter (→5 V/USB or board V_in) | Fuse it; mind **parasitic draw** so it can't flatten the bike battery |
| Backup | LiPo on the board's battery input | Keeps reporting if main power is cut (anti-theft) |

## Build/coexistence notes

- **Phase 1** runs on the **Arduino Nano ESP32** (ABX00083, ESP32-S3) I already own (BLE only).
- **Phase 2** runs on the **LilyGO T-SIM7670G-S3** (ESP32-S3 + modem + GPS + LiPo mgmt).
- **Phase 5 (optional)** merges Layer 1's advertising loop onto the Phase 2 board so both run together.
- BLE advertising and the cellular modem do not contend: BLE is the SoC radio; the modem is a
  separate chip on UART. The only care needed is serializing the two BLE *payloads* (Apple/Google)
  in one advertising loop rather than running two libraries that each assume exclusive control.

## Threat / reliability reasoning

- **Sealed garage / remote area:** Layer 1 goes silent (no finders). Layer 2 still works on cell + GPS — this is *why* Layer 2 is primary.
- **Underground concrete:** both GPS and BLE degrade; expect a gap until the bike resurfaces.
- **Thief cuts main power:** backup LiPo keeps Layer 2 alive long enough to report; motion already triggered an SMS at first movement.
- **Reverse-engineered networks:** Apple/Google can push changes that break Layer 1 firmware (Google side especially is early). Never rely on Layer 1 alone.
