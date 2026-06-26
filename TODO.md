# TODO

Living progress tracker for the motorcycle tracker build. Mirrors the phased plan in
[`README.md`](README.md) and [`docs/build-roadmap.md`](docs/build-roadmap.md).

Status key: `[ ]` planned · `[~]` in progress · `[x]` done · `[!]` blocked (human/procurement)

---

## Active session: Phase 0 setup → Phase 1a start

### Part A — Document the real board (docs housekeeping, `main`) — DONE (commit 218e23d)
- [x] Name the **Arduino Nano ESP32 (ABX00083)** in place of generic "ESP32-S3 dev board"
  - [x] `README.md` (hardware owned + repo-layout tree)
  - [x] `architecture.md` (build/coexistence note)
  - [x] `CLAUDE.md` (where-things-live + ESP-IDF project-wide toolchain note)
  - [x] `docs/shopping-list.md` (already-owned line)
  - [x] `docs/build-roadmap.md` (Phase 0/1 board + toolchain columns → ESP-IDF)
  - [x] `docs/component-ble-crowdsourced.md` (board name + esp32s3 target + py-pin caveat)
- [x] Commit the untracked `docs/arduino-nano-esp32-user-manual.md`

### Part B — Phase 0: ESP-IDF setup + hello-world (`phase-0-setup`)
- [x] Scaffold `firmware/hello-world/` (boot banner + chip info + heartbeat, USB-Serial/JTAG console)
- [x] Host-side Unity **logic** unit-test pipeline (`linux` target) — banner logic + expected
      strings independently verified with host gcc (5 cases incl. truncation)
- [x] Document ESP-IDF install on Windows (target esp32s3) in `firmware/hello-world/README.md`
- [!] **Owner action:** install ESP-IDF, then `idf.py set-target esp32s3 && idf.py -p COMx flash monitor`
      to confirm banner+heartbeat; and run host tests via `host_test/` (`idf.py --preview set-target linux`)

### Part C — Phase 1a: Apple Find My beacon (`phase-1-ble`) — start only
- [x] `apple_find_my` encoder component: key → BLE static addr + 31-byte adv payload
      (byte layout confirmed against current OpenHaystack/Macless-Haystack upstream)
- [x] Host-side Unity tests for Apple BLE adv payload construction
      (full layout + addr `|0xC0` masking + `key[0]>>6` high-bits; verified with host gcc)
- [x] Phase 1a runbook in `firmware/ble-beacon/README.md`
- [!] **Owner action:** clone Macless-Haystack, make a pinned 3.11/3.12 venv, generate keys
- [!] **Owner action:** port/build ESP32 firmware for esp32s3 (verify S3 portability), inject key, flash
- [!] **Owner action:** stand up viewer + anisette (Docker + Apple ID); confirm map pin

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
- [ ] ESP32-S3 firmware portability for Macless-Haystack → verify in Phase 1a
- [ ] Confirm Phase 2 toolchain (ESP-IDF `esp_modem` vs Arduino+TinyGSM) → at board purchase

---

_Last updated: 2026-06-26 — Phase 0 scaffolded; Phase 1a Apple encoder + tests done; awaiting owner hardware + Macless-Haystack integration._
