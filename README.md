# Motorcycle Tracker — Project Plan

A hidden GPS/location tracker for my own motorcycle, built on the ESP32-S3.
The device reports its location through **two independent layers**:

1. **Crowdsourced BLE** (Apple Find My + Google Find My Device) — free, no SIM, passive.
2. **Cellular + GPS** — the device's own GPS fix pushed over its own SIM. Real-time, on-demand, reliable.

These two layers are built in **separate phases** and can coexist later on one device.

> This repo is a set of planning docs written to be picked up by **Claude Code sessions**.
> Each `docs/component-*.md` is a self-contained work package: context, goal, tasks,
> acceptance criteria, references, and open questions. Start a session by reading
> `architecture.md`, then the relevant component doc.

---

## Hardware I already have

- **Arduino Nano ESP32** (ABX00083) — used for **Phase 1**. An ESP32-S3 inside a u-blox
  **NORA-W106-10B** module: native USB, built-in antenna, 8 MB PSRAM, VIN 6–21 V. See
  [`docs/arduino-nano-esp32-user-manual.md`](docs/arduino-nano-esp32-user-manual.md).

## Hardware to buy

See [`docs/shopping-list.md`](docs/shopping-list.md). Headline item for Phase 2 is the
**LilyGO T-SIM7670G-S3** (ESP32-S3 + LTE Cat-1 + GNSS all-in-one).

---

## Phased plan (top layer)

### Phase 0 — Project setup
- Repo scaffolding, toolchain (ESP-IDF and/or Arduino-ESP32), flashing the bare ESP32-S3.
- Decide build system per component (Phase 1 Google firmware is ESP-IDF; Phase 2 is easiest in Arduino + TinyGSM).
- **Done when:** I can flash and serial-monitor "hello world" on the ESP32-S3 I own.

### Phase 1 — Crowdsourced BLE layer (no SIM, uses existing ESP32-S3)
Build on the board I already have. See [`docs/component-ble-crowdsourced.md`](docs/component-ble-crowdsourced.md).
- **1a.** Apple Find My beacon via **Macless-Haystack** (most mature; weeks of keys, no re-registration). Verify/port for ESP32-S3 BLE.
- **1b.** Google Find My Device beacon via **GoogleFindMyTools** ESP32 firmware. Note 3-day re-registration caveat.
- **1c.** Time-multiplex both beacons in one advertising loop.
- **Done when:** the parked board shows up in the Apple tool (and ideally the Google tool) with a map pin, retrieved over my home internet.

### Phase 2 — Cellular + GPS layer (new all-in-one board)
See [`docs/component-cellular-gps.md`](docs/component-cellular-gps.md).
- **2a.** Bring up the T-SIM7670G-S3: modem responds to AT, registers on network, gets a GPS fix.
- **2b.** **Easiest reporting first:** send an **SMS with a Google Maps link** on demand. See [`docs/component-reporting.md`](docs/component-reporting.md).
- **2c.** Add **wake-on-motion** via accelerometer (LIS3DH) so the device sleeps until the bike moves, then fixes + reports.
- **Done when:** moving the device triggers an SMS to my phone containing a tappable map pin of its real location.

### Phase 3 — Power for unattended deployment
See [`docs/component-power.md`](docs/component-power.md).
- **3a.** Prototype on a USB powerbank (mind the auto-shutoff gotcha).
- **3b.** Tap the motorcycle 12 V battery via a buck converter, with fusing + low parasitic draw, plus a backup LiPo.
- **Done when:** the device survives weeks parked in public on bike power, with LiPo backup if main power is cut.

### Phase 4 — (Optional) richer reporting
- Self-hosted **Traccar** for a live browser/app map with history + geofence alerts, pushed over cellular data.
- Keep SMS as the instant "it's moving" alert. See [`docs/component-reporting.md`](docs/component-reporting.md).

### Phase 5 — (Optional) merge both layers on one board
- Run the Phase 1 BLE advertising loop alongside the Phase 2 cellular/GPS firmware on the T-SIM7670G-S3.
- BLE = free passive position while parked in public; cellular = real-time theft response.

---

## Guiding decisions (already settled in planning)

| Decision | Choice | Why |
|---|---|---|
| MCU | ESP32-S3 (Arduino Nano ESP32, owned) | Already owned; BLE 5.0; same toolchain across both phases |
| Build order | BLE first, then cellular | BLE needs no SIM/hardware purchase; reuse owned board |
| Cellular tech | **LTE Cat-1** (SIM7670G) | Broad coverage, no LTE-M/NB-IoT region check needed |
| Cellular board | LilyGO **T-SIM7670G-S3** | All-in-one ESP32-S3 + Cat-1 + GNSS + LiPo mgmt + eSIM pad |
| Easiest reporting | **SMS + maps link** | Zero backend; works as instant alert |
| Richer reporting | Traccar (later, optional) | Self-hosted live map + history, no app to build |
| Power (proto) | USB powerbank | Cheap, simple for bench work |
| Power (final) | Bike 12 V → buck + backup LiPo | Survives weeks unattended in public |
| Theft trigger | LIS3DH wake-on-motion | Enables low power + instant movement alert |

## Open decisions (carry into the relevant session)

- **SIM/eSIM provider** — must support **data + outbound SMS** and the right **LTE bands for my region**. Candidates: 1NCE, Soracom, Hologram, Twilio. (Decide in `component-cellular-gps.md` / `component-reporting.md`.)
- **Exact LilyGO SKU + band variant** — confirm the GPS-included version and correct regional bands before ordering.
- **Firmware portability to ESP32-S3** for the Phase 1 BLE projects — verify, may need minor porting.

---

## Repo layout

```
motorcycle-tracker/
├── README.md                         # this file — top-level plan + index
├── architecture.md                   # system architecture, layers, data flows
├── TODO.md                            # living progress tracker
├── firmware/                          # ESP-IDF firmware (added as phases are built)
└── docs/
    ├── build-roadmap.md             # phase/branch dispatch guide for sessions
    ├── arduino-nano-esp32-user-manual.md # datasheet for the owned Phase 1 board
    ├── shopping-list.md              # what to buy, with reasoning
    ├── component-ble-crowdsourced.md # Phase 1: Apple + Google BLE beacons
    ├── component-cellular-gps.md     # Phase 2: T-SIM7670G-S3, GPS, modem
    ├── component-reporting.md        # SMS now, Traccar later
    └── component-power.md            # powerbank → bike battery
```

## Legal / ethical note

This is for tracking **my own vehicle**. The crowdsourced beacons use rolling-key techniques
that overlap with anti-stalking-evasion; that is acceptable on my own property only. Do not use
this to track people or vehicles that aren't mine.
