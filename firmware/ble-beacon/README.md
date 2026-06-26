# Phase 1a — Apple Find My beacon (Arduino Nano ESP32 / ESP32-S3)

Make the owned Nano ESP32 advertise as an Apple Find My beacon so nearby iPhones report
its location, retrieved via the **Macless-Haystack** viewer over home internet — no SIM.
Component doc: [`../../docs/component-ble-crowdsourced.md`](../../docs/component-ble-crowdsourced.md).

**Acceptance:** a plausible map pin for the board appears in the Macless-Haystack viewer,
device holding no SIM.

> License note: `main/ble_beacon_main.c` is derived from Macless-Haystack (**AGPL-3.0**), so
> this firmware is AGPL-3.0.

---

## What's in this directory

- **`main/ble_beacon_main.c`** — the beacon: loads rolling advertisement keys from the `key`
  NVS partition, builds the BLE address + payload via the host-tested `apple_find_my`
  component, advertises a short burst, then deep-sleeps. Builds for **esp32s3 on ESP-IDF
  v6.0.1** (verified).
- **`components/apple_find_my/`** — the Find My advertisement encoder (key → 6-byte address +
  31-byte payload), matching OpenHaystack/Macless-Haystack byte-for-byte.
- **`partitions.csv`** — `factory` app + `key` (`nvs_keys`) partition at `0x110000`.
- Host tests for the encoder live in [`../test`](../test).

---

## 1. Host unit tests (no hardware)

```powershell
cmake -S firmware/test -B firmware/test/build -G "MinGW Makefiles"
cmake --build firmware/test/build
ctest --test-dir firmware/test/build --output-on-failure
```

## 2. Build the firmware (ESP-IDF PowerShell)

> ⚠️ Use a **clean** shell for `idf.py` — do **not** have the project `.venv` active at the
> same time. The ESP-IDF profile activates its own Python venv, whose `Activate.ps1` runs
> `deactivate` on the prior `.venv` and restores PATH to its pre-venv state, which strips the
> IDF tools (e.g. `ninja`) off PATH and makes `idf.py build/flash` fail with `WinError 2`.
> VS Code's integrated terminal auto-activates `.venv`, so run `deactivate` first (or use a
> separate non-VS-Code PowerShell / the EIM "ESP-IDF" desktop shortcut). Keep the `.venv` for
> the key-generation step below in its own terminal.

```powershell
idf.py set-target esp32s3
idf.py build
```

## 3. Generate keys (project `.venv`)

```powershell
.\.venv\Scripts\python.exe -m pip install cryptography      # one time
.\.venv\Scripts\python.exe external\macless-haystack\generate_keys.py -p tracker01
```

Outputs to the (git-ignored) `output/` folder:
- `tracker01_keyfile` — device key blob (1 count byte + N×28-byte advert keys) → flash at `0x110000`.
- `tracker01_devices.json` — import into the Macless-Haystack endpoint/app (holds the **private** keys).
- `tracker01.keys` — human-readable keys.

For real fit-and-forget coverage generate a set of rolling keys (the firmware rotates every
~30 min): `... generate_keys.py -p tracker01 -n 50 --thisisnotforstalking i_agree`.

> ⚠️ `output/`, `*_keyfile`, `*_devices.json`, `*.keys` contain **private keys** and are
> git-ignored — never commit them.

## 4. Flash (owner) — board over USB

Flashing ESP-IDF replaces the Arduino bootloader, so the **double-tap-RESET** recovery no
longer works. To (re)flash you must put the board in **firmware download mode** using the
**B1** test pad (= GPIO0), which has a **GND** pad right next to it
([Arduino guide](https://support.arduino.cc/hc/en-us/articles/9810414060188-Reset-the-Arduino-bootloader-on-the-Nano-ESP32)):

1. Short **B1 ↔ GND**. The RGB LED turns **green** — that confirms the *correct* pads (red
   means wrong pads / a short to 3V3).
2. While still shorted, press and release the white **RST** button.
3. Remove the jumper. The RGB LED stays **purple** (blue/yellow on early units) = download
   mode. A **stable COM port** now appears (use [`../../tools/watch-com-ports.ps1`](../../tools/watch-com-ports.ps1) to find it).

Then, from an ESP-IDF PowerShell (no `.venv`!) in this directory:

```powershell
idf.py -p COM<N> flash                                                       # app
python -m esptool --chip esp32s3 -p COM<N> write-flash 0x110000 ..\..\output\tracker01_keyfile
```

You only need the B1 dance **once**: after the app flashes, this firmware stays awake and
USB-reachable (see `BEACON_WAKE_WINDOW_S`, and it stays awake indefinitely while no keys are
present), so the second command — and future re-flashes — connect on their own via the COM
port. Press **RST** when done; monitor with `idf.py -p COM<N> monitor`.

## 5. Viewer (owner) — Docker + Apple ID

Bring up the Macless-Haystack endpoint + **anisette** server from
`external/macless-haystack/endpoint` (Docker), log in with an **Apple ID**, import
`tracker01_devices.json`, and confirm a map pin once the parked board has been near iPhones.

---

## Notes
- Apple precomputes weeks of rolling keys → fit-and-forget; this is the primary crowdsourced
  channel. Google FMDN (Phase 1b) is a bonus with ~3-day re-registration friction.
- Key rotation overlaps with anti-stalking-evasion; acceptable on one's own vehicle only.
