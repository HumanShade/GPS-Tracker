# TODO

Living progress tracker for the motorcycle tracker build. Mirrors the phased plan in
[`README.md`](README.md) and [`docs/build-roadmap.md`](docs/build-roadmap.md).

Status key: `[ ]` planned · `[~]` in progress · `[x]` done · `[!]` blocked (human/procurement)

---

## Phase 0 → Phase 1a

Toolchain is now installed (ESP-IDF v6.0.1) and the firmware **builds for esp32s3 for real**;
`main` + `phase-0-setup` are pushed to origin. Remaining Phase 1a work is owner hardware + viewer.

### Part A — Document the real board (docs housekeeping, `main`) — DONE (commit 218e23d, pushed)
- [x] Name the **Arduino Nano ESP32 (ABX00083)** in place of generic "ESP32-S3 dev board"
  - [x] `README.md` (hardware owned + repo-layout tree)
  - [x] `architecture.md` (build/coexistence note)
  - [x] `CLAUDE.md` (where-things-live + ESP-IDF project-wide toolchain note)
  - [x] `docs/shopping-list.md` (already-owned line)
  - [x] `docs/build-roadmap.md` (Phase 0/1 board + toolchain columns → ESP-IDF)
  - [x] `docs/component-ble-crowdsourced.md` (board name + esp32s3 target + py-pin caveat)
- [x] Commit the untracked `docs/arduino-nano-esp32-user-manual.md`

### Part B — Phase 0: ESP-IDF setup + hello-world (`phase-0-setup`, pushed)
- [x] Scaffold `firmware/hello-world/` (boot banner + chip info + heartbeat, USB-Serial/JTAG console)
- [x] **Builds clean for esp32s3 on IDF v6.0.1** (verified, 85% app partition free)
- [x] Document ESP-IDF install on Windows in `firmware/hello-world/README.md`
- [!] **Owner action:** `idf.py -p COMx flash monitor` to confirm banner+heartbeat on hardware

### Part C — Phase 1a: Apple Find My beacon (`phase-1-ble`, not pushed yet)
- [x] `apple_find_my` encoder component (byte layout confirmed vs current upstream)
- [x] **Portable** host test harness `firmware/test/` (CMake + vendored Unity; runs natively —
      the IDF `linux` target does NOT build on Windows). banner + apple_find_my pass via ctest.
- [x] Beacon firmware `firmware/ble-beacon/` **builds for esp32s3 on IDF v6** (port resolved:
      16MB flash, BLE 4.2 legacy adv, classic-BT release guarded out)
- [x] Keys generated (`output/tracker01_keyfile` etc.) via `generate_keys.py` in `.venv`
- [x] Runbook updated in `firmware/ble-beacon/README.md`
- [!] **Owner action:** `idf.py -p COMx flash` + flash `tracker01_keyfile` at `0x110000`
- [!] **Owner action:** stand up endpoint + anisette (Docker + Apple ID), import devices.json, confirm map pin

---

## Backlog (later phases — see README/roadmap)
- [ ] Phase 1b Google FMDN beacon (bonus; ~3-day re-registration; py-pin risk)
- [ ] Phase 1c time-multiplex Apple + Google in one advertising loop
- [!] Phase 2 Cellular/GPS — needs T-SIM7670G-S3 purchase + SIM w/ data+SMS (toolchain: lean ESP-IDF `esp_modem`)
- [ ] Phase 3 Power (bike 12 V buck + backup LiPo) — after Phase 2 hardware
- [ ] Phase 4 Traccar (optional)
- [ ] Phase 5 Merge BLE + cellular on one board (optional)
- [ ] **HIL / on-target / long-term testing** — explicitly its own later phase

---

## Open decisions to resolve in-phase
- [ ] SIM/eSIM provider + plan (data + outbound SMS + regional bands) → Phase 2
- [ ] Exact LilyGO SKU + band variant (GPS-included, Standard edition) → before ordering
- [x] ESP32-S3 firmware portability for Macless-Haystack → **RESOLVED**: builds for esp32s3/IDF v6
- [ ] Confirm Phase 2 toolchain (ESP-IDF `esp_modem` vs Arduino+TinyGSM) → at board purchase

---

_Last updated: 2026-06-26 — Phase 0 + Phase 1a firmware build clean for esp32s3 on IDF v6; host tests pass; keys generated. Remaining: owner flashes + stands up the Apple viewer._
