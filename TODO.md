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

### Part C — Phase 1a: Apple Find My beacon (`phase-1-ble`, pushed)
- [x] `apple_find_my` encoder component (byte layout confirmed vs current upstream)
- [x] **Portable** host test harness `firmware/test/` (CMake + vendored Unity; runs natively —
      the IDF `linux` target does NOT build on Windows). banner + apple_find_my pass via ctest.
- [x] Beacon firmware `firmware/ble-beacon/` **builds for esp32s3 on IDF v6** (port resolved:
      16MB flash, BLE 4.2 legacy adv, classic-BT release guarded out)
- [x] Post-reset wake window + stay-awake-when-unkeyed (re-flash without the B1 jumper);
      optional `BEACON_NO_DEEP_SLEEP` continuous mode for bring-up
- [x] Keys generated (`output/tracker01_keyfile` etc.) via `generate_keys.py`
- [x] **Flashed to hardware and advertising** — `Found 1 keys`, address `0xE7..` = `key[0]|0xC0`,
      `advertising has started` on the Nano ESP32 (via B1 download mode → `idf.py flash` + keyfile @0x110000)
- [x] Endpoint setup documented: [`docs/findmy-endpoint-setup.md`](docs/findmy-endpoint-setup.md)
- [!] **BLOCKED (Apple-side):** endpoint login fails on the **Apple account trust-score gate**
      (`com.apple.mobileme status 1`). Card+address added 2026-06-27, still blocked. Next:
      sign the Apple ID into a real Apple device, and/or wait for the score to update, then
      `docker start -ai macless-haystack`. See the endpoint doc.
- [ ] Import `devices.json` into the frontend and confirm a map pin (after the gate clears)

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

_Last updated: 2026-06-27 — Phase 1a beacon flashed and advertising on hardware (key + address verified). Only remaining blocker is the Apple account trust-score gate on the Macless-Haystack endpoint login._
