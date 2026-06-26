# Shopping List

Prices are rough and region-dependent — **verify current price, stock, and regional band
variant before ordering.** Items are grouped by phase so you can buy as you go.

## Already owned
- **Arduino Nano ESP32** (ABX00083) — ESP32-S3 in a u-blox NORA-W106-10B module. Used for
  Phase 1 (BLE). No purchase needed. See [`arduino-nano-esp32-user-manual.md`](arduino-nano-esp32-user-manual.md).

---

## Phase 1 — Crowdsourced BLE (uses owned ESP32-S3)
Nothing strictly required beyond the board you have and a way to power it (USB).
Optional:
- **USB powerbank** (if you don't already have one) — for bench testing untethered. See power notes.

> You also need an **Apple ID** (to view via Macless-Haystack) and a **Google account**
> (for GoogleFindMyTools). No cost.

---

## Phase 2 — Cellular + GPS

### Core board (pick the GPS variant)
- **LilyGO T-SIM7670G-S3** — ESP32-S3 + **SIM7670G (LTE Cat-1)** + **GNSS** + LiPo charging + eSIM pad, all on one board.
  - ✅ Reason: easiest cellular path. Cat-1 = broad LTE coverage with no LTE-M/NB-IoT region check. Same ESP32-S3 toolchain you're already in. All-in-one removes modem wiring, level shifting, and antenna/power headaches.
  - ⚠️ **Confirm:** the **"with GPS" SKU**, the **correct LTE bands for your country**, and whether you want the **Standard** edition (adds seamless power switching, GNSS routed to the SoC, eSIM pad).
  - Alternative if you later optimize hard for battery and have LTE-M/NB-IoT coverage: **T-SIM7000G-S3** (lower power, but verify coverage). Not recommended for the "easiest" path.

### Antennas
- **LTE antenna** and **GNSS (GPS) antenna** — often **included** with the LilyGO board; confirm in the listing. If buying separately, match the connector (usually u.FL/IPEX) and get an **active GPS antenna** for the GNSS port.

### SIM / connectivity
- **IoT SIM or eSIM** with **data + outbound SMS**, correct region bands. Candidates:
  - **1NCE** — flat multi-year fee, simple, good for low-volume IoT. Confirm SMS support.
  - **Soracom** — flexible, good dashboards, eSIM options.
  - **Hologram**, **Twilio** — well-documented, global.
  - ⚠️ Many cheap **data-only** IoT SIMs **don't do SMS** — verify, since the easiest reporting path is SMS.

### Theft trigger
- **LIS3DH accelerometer breakout** (or ADXL345) — I²C, hardware **wake-on-motion** interrupt.
  - Connects to the ESP32-S3 I²C pins + one GPIO for the INT line.
  - Can be added in step 2c, after basic SMS works.

---

## Phase 3 — Power (production)
- **Buck converter / DC-DC** rated for **automotive input** (handles ~12–14.4 V, ideally load-dump tolerant), output to 5 V/USB or the board's battery input.
  - Look for an **automotive/"car" buck** or add input protection (TVS, reverse-polarity diode).
- **Inline fuse + holder** (low value, e.g. 1–2 A) for the 12 V tap.
- **LiPo battery** (e.g. 1S 3.7 V, capacity to taste) for backup if main power is cut. The LilyGO board has LiPo charging built in.
- **Wire, connectors, heatshrink**, and a **small enclosure** (consider non-metal for BLE/GPS, hidden mounting under the seat).

---

## Nice-to-have / dev tooling
- **USB-C cable** suited to the LilyGO board's serial chip (CH9102) — for flashing/monitoring.
- **microSD card** (the LilyGO board has a slot) — optional local logging.
- **Multimeter** — essential for the power phase (measuring parasitic draw on the bike battery).

---

## Quick reasoning recap

| Item | Why this one |
|---|---|
| T-SIM7670G-S3 | Cat-1 broad coverage + GPS + ESP32-S3 + LiPo mgmt, one board = easiest cellular |
| LIS3DH | Standard wake-on-motion IRQ; enables low power + instant alert |
| IoT SIM w/ SMS | SMS is the zero-backend reporting path; must not be data-only |
| Automotive buck + fuse | Survives 12 V bike electrical environment without frying the board or bike battery |
| Backup LiPo | Keeps reporting if a thief cuts main power |
