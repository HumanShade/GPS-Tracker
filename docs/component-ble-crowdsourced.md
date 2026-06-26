# Component: Crowdsourced BLE Layer (Phase 1)

**Goal:** make the ESP32-S3 I already own appear as a trackable beacon on **Apple Find My**
and **Google Find My Device**, so a parked bike in a public space gets passively located by
strangers' phones — no SIM, no subscription, near-zero power.

**Board:** the **Arduino Nano ESP32** (ABX00083, ESP32-S3 / u-blox NORA-W106-10B) I already
have — see [`arduino-nano-esp32-user-manual.md`](arduino-nano-esp32-user-manual.md).
**Toolchain:** ESP-IDF, target `esp32s3`.
**Cost:** none (uses owned hardware + free Apple/Google accounts).

---

## Context for the session

- This layer only **broadcasts BLE advertisements**. Nearby phones report the location to
  Apple/Google; I read it back with project tools over home internet.
- Two separate networks = two packet formats. Build them **one at a time**, then combine.
- **Apple is the more mature path** (precomputes weeks of rolling keys, no re-registration) →
  do Apple first so there's a working result early.
- ✅ **ESP32-S3 portability — RESOLVED for Apple (Phase 1a).** The Macless-Haystack ESP32
  firmware was ported to `firmware/ble-beacon/` and **builds for `esp32s3` on ESP-IDF v6.0.1**.
  Three changes were needed: (1) set 16 MB flash (`CONFIG_ESPTOOLPY_FLASHSIZE_16MB`) so the
  `key` partition at `0x110000` fits; (2) enable BLE 4.2 legacy advertising
  (`CONFIG_BT_BLE_42_FEATURES_SUPPORTED`) — off by default on BLE-5.0 chips, but this firmware
  uses the legacy `esp_ble_gap_*_advertising` APIs; (3) guard out the classic-BT memory release
  (`#if CONFIG_IDF_TARGET_ESP32`), since the S3 is BLE-only. On-hardware advertising is still
  owner-verified. GoogleFindMyTools (Phase 1b) S3 support remains to be checked.
- ⚠️ **Host Python version:** system Python is 3.14 (too new for some Find My host tooling, e.g.
  Macless-Haystack key generation and the anisette/viewer stack). Run host scripts in a **pinned
  3.11/3.12 venv**, not system Python.

---

## 1a — Apple Find My beacon (Macless-Haystack)

**Tasks**
1. Get **Macless-Haystack** (maintained fork of OpenHaystack). Read its README for the current flow.
2. Generate a keypair / set of rolling keys for the device.
3. Build + flash the ESP32-S3 beacon firmware. **Confirm the build targets ESP32-S3**; adjust the
   target / SDK config if the project defaults to classic ESP32.
4. Stand up the Macless-Haystack viewer/server component (it resolves keys → location, with a map).
5. Power the board, leave it where phones pass by, and confirm a pin appears in the viewer.

**Acceptance**
- The board shows up in the Macless-Haystack viewer with a plausible map location, retrieved over
  my home internet, with no SIM in the device.

**Notes**
- Apple's network precomputes many rolling keys → can run for **weeks unattended**. This is the
  fit-and-forget channel.
- Key rotation overlaps with anti-stalking-evasion; fine on my own vehicle only.

---

## 1b — Google Find My Device beacon (GoogleFindMyTools)

**Tasks**
1. Get **GoogleFindMyTools** (Leon Böttger). It has a `main.py` locator script + ESP-IDF C firmware.
2. Install the Python deps (`pip install -r requirements.txt`); have Chrome available (used for login).
3. Run the script, log into Google, register the ESP32 tracker, obtain the **advertisement key**.
4. Paste the advertisement key into the firmware source (the README points to the exact line),
   build with **ESP-IDF**, flash the ESP32-S3.
5. Run `main.py` again and confirm the board is locatable.

**Acceptance**
- The board's location is retrievable via the GoogleFindMyTools script.

**Known caveats (document, don't fight)**
- **~3-day re-registration:** keys may need refreshing every few days (re-run the script, host-side;
  the device keeps advertising). This is a limitation of the early re-implementation.
- **No MAC randomization yet** → more trackable than ideal; acceptable for experimentation.
- Located via the **Python script**, not Google's native app.
- Because of the re-registration friction, treat Google as a **bonus** and Apple as the primary
  crowdsourced channel.

---

## 1c — Time-multiplex both beacons

**Goal:** one BLE advertising loop that alternates the Apple-format and Google-format payloads.

**Tasks**
1. Merge the two beacon behaviors into a **single advertising scheduler** rather than running two
   libraries that each assume exclusive control of the advertiser.
2. Rotate payloads on a short interval (e.g. swap every ~300–500 ms): set Apple payload → advertise →
   set Google payload → advertise → repeat, each maintaining its own rolling-key logic.
3. Confirm **both** tools still locate the device while multiplexing.

**Acceptance**
- With one firmware running, the device is visible in **both** the Apple viewer and the Google script.

**Notes**
- The radio only advertises one packet at a time; multiplexing at a few hundred ms is invisible to
  both networks.
- Keep this loop low-duty so it survives deep-sleep-ish operation for the eventual battery build.

---

## References (fetch latest in-session)
- Macless-Haystack (GitHub) — maintained OpenHaystack fork; current flashing/viewer flow.
- GoogleFindMyTools by Leon Böttger (GitHub) — ESP32 firmware + `main.py`.
- Background: Find My / Find Hub protocol research (cc-sw.com), Hackaday/CNX coverage of the ESP32 Google port.

## Open questions
- Does the current Macless-Haystack firmware build cleanly for **ESP32-S3**, or is a port needed?
- Is the Google 3-day re-registration acceptable for a fit-and-forget tracker, or should Google be
  dropped from the always-on build and only enabled situationally?
