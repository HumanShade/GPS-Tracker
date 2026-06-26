# TODO

Living progress tracker for the motorcycle tracker build. Mirrors the phased plan in
[`README.md`](README.md) and [`docs/build-roadmap.md`](docs/build-roadmap.md).

Status key: `[ ]` planned · `[~]` in progress · `[x]` done · `[!]` blocked (human/procurement)

---

## Active session: Phase 0 setup → Phase 1a start

### Part A — Document the real board (docs housekeeping, `main`)
- [~] Name the **Arduino Nano ESP32 (ABX00083)** in place of generic "ESP32-S3 dev board"
  - [ ] `README.md` (hardware owned + repo-layout tree)
  - [ ] `architecture.md` (build/coexistence note)
  - [ ] `CLAUDE.md` (where-things-live + ESP-IDF project-wide toolchain note)
  - [ ] `docs/shopping-list.md` (already-owned line)
  - [ ] `docs/build-roadmap.md` (Phase 0/1 board + toolchain columns → ESP-IDF)
  - [ ] `docs/component-ble-crowdsourced.md` (board name + esp32s3 target + py-pin caveat)
- [ ] Commit the untracked `docs/arduino-nano-esp32-user-manual.md`

### Part B — Phase 0: ESP-IDF setup + hello-world (`phase-0-setup`)
- [ ] Install + document ESP-IDF on Windows (target esp32s3)
- [ ] Scaffold `firmware/hello-world/` (boot banner + chip info + heartbeat, USB-Serial/JTAG console)
- [ ] Host-side Unity **logic** unit-test pipeline (`linux` target)
- [!] Owner flashes board: `idf.py -p COMx flash monitor` shows banner (physical, owner-run)

### Part C — Phase 1a: Apple Find My beacon (`phase-1-ble`) — start only
- [ ] Fetch latest Macless-Haystack upstream
- [ ] Generate keys in pinned 3.11/3.12 venv (not system 3.14)
- [ ] Port/build ESP32 firmware for esp32s3 (verify S3 portability)
- [ ] Host-side Unity tests for Apple BLE adv payload construction
- [!] Stand up viewer + anisette (needs Docker + Apple ID); confirm map pin

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
