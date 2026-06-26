# Build Roadmap

A dispatch guide for picking up this project one phase at a time. Each phase is a
self-contained work package backed by a component doc; phases are built on their own
git branch so work can proceed (and be reviewed) independently.

## Starting a work session
1. Read [`../architecture.md`](../architecture.md) for the two-layer system overview.
2. Read the component doc for the phase you're taking on (linked in the table below).
3. Branch off `main` using the branch name in the table. Keep one phase per branch.
4. Fetch the latest upstream for any external project the phase integrates — the docs
   reference projects that move quickly, so don't rely on cached instructions.

## Phases, branches, and dependencies

| Phase | Branch | Board | Toolchain | Component doc | Depends on | Parallel? |
|---|---|---|---|---|---|---|
| 0 Setup | `phase-0-setup` | Arduino Nano ESP32 | ESP-IDF | (README Phase 0) | — | first, blocks all |
| 1 BLE | `phase-1-ble` | Arduino Nano ESP32 | ESP-IDF | [component-ble-crowdsourced.md](component-ble-crowdsourced.md) | 0 | ∥ with Phase 2 |
| 2 Cellular/GPS | `phase-2-cellular` | T-SIM7670G-S3 | ESP-IDF (`esp_modem`) | [component-cellular-gps.md](component-cellular-gps.md) | 0 (+ board purchase) | ∥ with Phase 1 |
| 3 Power | `phase-3-power` | T-SIM7670G-S3 | hardware | [component-power.md](component-power.md) | 2 | after 2 |
| 4 Traccar | `phase-4-traccar` | T-SIM7670G-S3 | ESP-IDF (data uplink) | [component-reporting.md](component-reporting.md) | 2 | optional |
| 5 Merge | `phase-5-merge` | T-SIM7670G-S3 | ESP-IDF (both layers) | (README Phase 5) | 1 + 2 | optional, last |

## Sequencing notes
- **Toolchain is ESP-IDF project-wide** (target `esp32s3`) — a deliberate choice for a
  professional build with native unit testing. The owned Phase 0/1 board is the
  **Arduino Nano ESP32** (ESP32-S3); flashing ESP-IDF replaces its factory Arduino bootloader.
- **Phase 0 first, and it blocks everything.** "Done" = flash and serial-monitor a
  hello-world on the owned Arduino Nano ESP32.
- **Phase 1 and Phase 2 are independent** — different boards — so two sessions can run them
  on separate branches at the same time.
- **Phase 2 has procurement/human blockers, not just code:** buying the T-SIM7670G-S3
  (correct GPS SKU + regional LTE bands) and choosing a SIM/eSIM provider that does
  **data + outbound SMS**. Surface these as blockers rather than coding around them.
- **Phase 3 and 5 are downstream of the cellular board** — don't start them until
  Phase 2 hardware is in hand and working.
- **Much of Phase 1/2 is integration, not greenfield code:** Macless-Haystack and
  GoogleFindMyTools (Phase 1), the SIM7670G modem bring-up via `esp_modem` (Phase 2), plus
  hardware flashing. Expect research, porting, and verification work as much as new firmware.

## Open decisions to resolve in-phase
Carried from [`../README.md`](../README.md); resolve each in the phase that needs it:
- **SIM/eSIM provider + plan** (data + SMS + regional bands) → Phase 2 / reporting.
- **Exact LilyGO SKU + band variant** (confirm GPS-included, Standard edition) → before
  ordering for Phase 2.
- **ESP32-S3 firmware portability** for the Phase 1 BLE projects → verify early in Phase 1.
