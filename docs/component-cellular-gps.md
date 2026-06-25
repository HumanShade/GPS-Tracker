# Component: Cellular + GPS Layer (Phase 2)

**Goal:** the device gets its own GPS fix and reports it over its own SIM — reliable, real-time,
on-demand location independent of any nearby phones. The **easiest** reporting path (SMS with a
maps link) comes first.

**Board:** LilyGO **T-SIM7670G-S3** (ESP32-S3 + SIM7670G LTE Cat-1 + GNSS + LiPo charging).
**Library:** LilyGO's **TinyGSM fork** (the upstream master does not support SIM7670G cleanly).

---

## Why this board / module

- **LTE Cat-1** = broad coverage on standard LTE, **no LTE-M/NB-IoT region check** → the "just works" choice.
- **All-in-one ESP32-S3** keeps the same toolchain as Phase 1 and removes modem wiring, level
  shifting, antenna routing, and power-management work.
- Built-in **LiPo charging** feeds straight into the Phase 3 power plan.
- ⚠️ Confirm the **GPS-included SKU** and **correct LTE bands for your region** before ordering.

---

## 2a — Modem + GPS bring-up

**Tasks**
1. Install **Arduino-ESP32** (easiest for this phase) and LilyGO's **TinyGSM fork** + their board examples.
2. Flash a basic example; over serial, confirm the modem answers **AT** commands.
3. Power on the modem, register on the cellular network with the SIM (verify signal + registration).
4. Enable **GNSS** and acquire a **GPS fix** (lat/lon/time). Outdoors with sky view for first fix.
5. Log the fix over serial.

**Acceptance**
- Serial shows a valid network registration **and** a real GPS fix.

**Notes**
- First GPS fix outdoors can take a while (cold start). The bike's metal blocks signal — antenna
  placement with sky view matters even in testing.
- If LilyGO's library examples misbehave, falling back to **raw AT commands** over UART is a reliable
  path (well-trodden for this module).

---

## 2b — Easiest reporting: SMS with a maps link

> Full detail in [`component-reporting.md`](component-reporting.md). Summary here for the build order.

**Tasks**
1. On demand (initially: a button press or serial command), compose an SMS:
   `https://maps.google.com/?q=<lat>,<lon>`
2. Send it to my phone number via the modem (`sendSMS`).
3. Tap the link on the phone → opens Google/Apple Maps with a pin.

**Acceptance**
- Triggering the device sends an SMS that opens a correct map pin on my phone.

**Notes**
- Zero backend. This is both the MVP and the permanent **instant alert** channel.
- ⚠️ Watch the classic bug of concatenating lat/lon without a separator — build the string carefully
  (`String(lat) + "," + String(lon)`), include enough decimal places (≥5–6).
- Requires the SIM to support **outbound SMS** (not all data-only IoT SIMs do).

---

## 2c — Wake-on-motion (LIS3DH)

**Goal:** sleep until the bike physically moves, then wake, fix, and report. This is the hinge for
both battery life and instant theft alerts.

**Tasks**
1. Wire **LIS3DH** over I²C; route its **INT** pin to an ESP32-S3 wake-capable GPIO.
2. Configure the LIS3DH motion/activity interrupt threshold.
3. Put the ESP32-S3 in **deep sleep**, configured to wake on the LIS3DH INT (ext0/ext1 wake).
4. On wake: power up modem + GNSS → get fix → send SMS → return to sleep.
5. Tune the threshold so normal vibration/wind doesn't false-trigger but a real push/roll does.

**Acceptance**
- A stationary device stays asleep (low current); a physical move wakes it and produces an SMS with
  a fresh fix within seconds.

**State machine**
```
DEEP_SLEEP ──(LIS3DH motion IRQ)──▶ WAKE ──▶ MODEM_ON ──▶ GNSS_FIX ──▶ REPORT(SMS) ──▶ DEEP_SLEEP
                                                  │
                                          (optional: keep awake & stream
                                           fixes while motion continues)
```

**Notes**
- Optionally stay awake and send periodic updates while motion persists (theft in progress), then
  sleep after it's been still for N seconds.
- This step pairs directly with the power phase.

---

## References (fetch latest in-session)
- LilyGo-Modem-Series (GitHub, Xinyuan-LilyGO) — examples + TinyGSM fork + model comparison + band support.
- Board product page for the exact SKU/bands.
- TinyGSM docs for SMS + GPRS/TCP usage.
- LIS3DH datasheet + an Arduino LIS3DH motion-interrupt example.

## Open questions
- Which **SIM/eSIM provider + plan** (must do data **and** SMS, right bands)? Decide with `component-reporting.md`.
- Standard vs non-standard T-SIM7670G-S3 edition (Standard adds seamless power switching + GNSS-to-SoC + eSIM pad — relevant to Phase 3 power and Phase 5 merge).
