# Phase 1a — Apple Find My beacon (Arduino Nano ESP32 / ESP32-S3)

Make the owned Nano ESP32 advertise as an Apple Find My beacon so nearby iPhones report
its location, retrieved via the **Macless-Haystack** viewer over home internet — no SIM.
Component doc: [`../../docs/component-ble-crowdsourced.md`](../../docs/component-ble-crowdsourced.md).

**Acceptance:** a plausible map pin for the board appears in the Macless-Haystack viewer,
device holding no SIM.

---

## What's done in-repo (verified)

- **`components/apple_find_my/`** — the Find My advertisement encoder: advertisement key →
  6-byte BLE static address + 31-byte advertisement payload. Byte layout matches
  OpenHaystack / Macless-Haystack (`set_addr_from_key` / `set_payload_from_key`).
- **Host tests** ([`../test`](../test)) — Unity tests asserting the exact address and payload
  bytes (incl. the `key[0] | 0xC0` address masking and `key[0] >> 6` high-bits byte). Run with:
  ```powershell
  cmake -S firmware/test -B firmware/test/build -G "MinGW Makefiles"
  cmake --build firmware/test/build
  ctest --test-dir firmware/test/build --output-on-failure
  ```

This isolates the most error-prone, deterministic piece (the wire format) behind unit
tests so the on-device firmware can simply feed it the current rolling key.

---

## Owner / integration steps (next)

These need network, Docker, an Apple ID, and the board — run them yourself.

1. **Fetch latest Macless-Haystack** (it moves; don't rely on cached instructions):
   `git clone https://github.com/dchristl/macless-haystack`.
2. **Pinned Python venv** — system Python is 3.14 (too new for the tooling). Create a
   **3.11/3.12** venv and install the project requirements there.
3. **Generate keys** with the project's generator → yields the device key record and the
   advertisement key the firmware advertises.
4. **Firmware:** start from the project's ESP32 ESP-IDF firmware; **`idf.py set-target
   esp32s3`**; inject the advertisement key. ⚠️ Verify/port for ESP32-S3 (it targeted the
   classic ESP32) — record the outcome in the component doc. Reuse
   `components/apple_find_my` here rather than re-deriving the payload bytes.
5. **Viewer/endpoint:** bring up the Macless-Haystack endpoint + **anisette** server
   (Docker) and query with an **Apple ID**.
6. **Flash + verify:** flash the Nano ESP32 (see Phase 0 README for flashing + download-mode
   notes), leave it where phones pass, and confirm a pin in the viewer.

### Prerequisites checklist
- [ ] Docker (endpoint + anisette)
- [ ] Apple ID (viewer queries Apple's servers)
- [ ] Pinned Python 3.11/3.12 venv
- [ ] ESP-IDF installed (Phase 0)

---

## Notes
- Apple precomputes weeks of rolling keys → fit-and-forget; this is the primary crowdsourced
  channel. Google FMDN (Phase 1b) is a bonus with ~3-day re-registration friction.
- Key rotation overlaps with anti-stalking-evasion; acceptable on one's own vehicle only.
