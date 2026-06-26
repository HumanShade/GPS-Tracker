# CLAUDE.md

Guidance for Claude Code working in this repository.

## What this project is

A hidden GPS/location tracker for the owner's **own motorcycle**, built on the **ESP32-S3**.
Location is reported through two independent layers:

1. **Crowdsourced BLE** (Apple Find My + Google Find My Device) — free, no SIM, passive.
2. **Cellular + GPS** — the device's own GPS fix pushed over its own SIM. Real-time, on-demand.

The repo is currently **planning docs**; firmware is built phase by phase (see below).

## Where things live

- `README.md` — top-level plan and index. Start here.
- `architecture.md` — the two-layer system architecture and data flows.
- `docs/build-roadmap.md` — phase → branch dispatch guide. **Read this before starting build work.**
- `docs/component-*.md` — self-contained work packages (context, goal, tasks, acceptance).
- `docs/shopping-list.md` — hardware to buy, with reasoning.
- `docs/arduino-nano-esp32-user-manual.md` — datasheet for the owned Phase 1 board
  (Arduino Nano ESP32, ABX00083; ESP32-S3 in a u-blox NORA-W106-10B module).
- `TODO.md` — living progress tracker for the build.

When adding firmware, keep it out of the doc roots: use a top-level `firmware/` (or
per-board subdirs) rather than scattering source next to the planning docs.

## How to start a build session

1. Read `architecture.md`, then the component doc for the phase you're taking on.
2. Branch off `main` using the branch name from `docs/build-roadmap.md` (one phase per branch).
3. Fetch the latest upstream for any external project the phase integrates
   (Macless-Haystack, GoogleFindMyTools, LilyGO TinyGSM fork) — they move fast; don't rely
   on cached instructions.

## Phase / toolchain quick reference

- **Toolchain: ESP-IDF, project-wide** (target `esp32s3`) — chosen for a professional build.
- **Phase 0** must come first and blocks all others (flash + serial-monitor hello-world).
- **Phase 1 (BLE)** and **Phase 2 (cellular)** are independent — different boards — and can
  run in parallel on separate branches.
- Phase 0 + Phase 1 BLE → **ESP-IDF** on the owned **Arduino Nano ESP32** (ESP32-S3). Phase 2
  cellular → ESP-IDF on the T-SIM7670G-S3, modem via **`esp_modem`** (confirm at board
  purchase; the older Arduino+TinyGSM path is dropped). Phases 3/5 are downstream of the
  cellular board.
- Some blockers are **procurement/human**, not code (buy the T-SIM7670G-S3 with the right
  GPS SKU/bands; pick a SIM that does data **and** outbound SMS). Surface these; don't code around them.

## Working conventions

- **Commits:** commit incrementally as work progresses so history lands in the repo.
  Messages are short, neutral, in the imperative, and relevant to a new reader — **never**
  reference the chat/session. Do **not** add a `Co-Authored-By: Claude` trailer.
- **Branches:** firmware work goes on a `phase-*` branch, not directly on `main`. Docs-only
  housekeeping on `main` is fine.
- **Docs:** these markdown files are the source of truth for intent. If a build decision
  changes the plan, update the relevant doc in the same change.
- **Open decisions** (SIM provider, exact LilyGO SKU/bands, ESP32-S3 firmware portability)
  are tracked in `README.md` / component docs — resolve each in the phase that needs it and
  record the outcome there.

## Legal / ethical

This tracks the **owner's own vehicle only**. The crowdsourced beacons use rolling-key
techniques that overlap with anti-stalking-evasion; that is acceptable on one's own property.
