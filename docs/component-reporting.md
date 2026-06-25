# Component: Reporting (how location reaches me)

Two layers report in completely different ways. The crowdsourced layer reports itself through
Apple/Google tools (nothing to build). This doc covers the **cellular layer's** reporting, built
in order of increasing effort.

---

## Crowdsourced layer (no work)
- **Apple:** Macless-Haystack viewer shows a map pin.
- **Google:** GoogleFindMyTools `main.py` prints/maps the latest fix.
- Already covered in [`component-ble-crowdsourced.md`](component-ble-crowdsourced.md). No server, no app.

---

## Step 1 (easiest, build first): SMS + maps link — zero backend

**What:** on a trigger, the modem texts me a Google Maps URL with the coordinates.

```
https://maps.google.com/?q=<lat>,<lon>
```

Tapping it opens Google **or** Apple Maps with a pin (the `?q=lat,lon` form works on both).

**Why first**
- No server, no frontend, no account beyond the SIM.
- Works on any phone, even a dumb phone.
- Perfect as the **instant "your bike is moving" alert.**

**Tasks**
1. Build the URL string carefully: `"https://maps.google.com/?q=" + String(lat,6) + "," + String(lon,6)`.
2. `modem.sendSMS(MY_NUMBER, body)`.
3. Trigger initially by button/serial; later by the LIS3DH wake-on-motion event.

**Acceptance**
- Trigger → I receive an SMS → tapping the link shows the correct location.

**Gotchas**
- SIM must support **outbound SMS** (many data-only IoT SIMs don't).
- Use ≥5–6 decimal places; don't forget the comma between lat and lon.
- SMS is an **alert/snapshot**, not continuous tracking. For "watch it move live," see Step 2.

---

## Step 2 (optional, later): Traccar — live map, history, geofence

**What:** the device pushes fixes over **cellular data** to a self-hosted **Traccar** server, which
gives a **browser dashboard + native iOS/Android apps**, route history, and geofence alerts — no
frontend to write.

**Why**
- "Browser-based app / maps pin with history" experience, fully self-hosted and under my control.
- Traccar speaks many device protocols out of the box, including a dead-simple HTTP one.

**Tasks**
1. Self-host Traccar (cheap VPS, or a Pi at home with a port forward / tunnel).
2. Use the **OsmAnd protocol**: the device sends an HTTP GET with `id`, `lat`, `lon`, `timestamp`
   to the Traccar endpoint. Simplest possible integration — no MQTT broker needed to start.
   - (MQTT is an alternative uplink if you'd rather; OsmAnd-over-HTTP is the least-effort start.)
3. Register the device in Traccar; confirm fixes land and the map updates.
4. Add a **geofence** around "home/usual parking" → alert when it leaves.

**Acceptance**
- Moving the device updates a live pin in the Traccar web UI / app, with a visible track and a
  geofence-exit alert.

**Notes**
- Requires a **data** plan on the SIM (any IoT data SIM).
- Keep **Step 1 SMS as the instant alert** even after Traccar exists: SMS taps me on the shoulder,
  Traccar is the detailed map I open once I'm paying attention.

---

## Step 3 (optional): custom broker + page
Only if Traccar lacks something specific: device publishes **MQTT** to a broker, a minimal
Leaflet/Google Maps page subscribes and renders pins. More flexible, but now I'm maintaining a
frontend. Skip unless needed.

---

## Recommended end state
- **SMS (Step 1)** = instant theft alert with a tappable pin.
- **Traccar (Step 2)** = live browser/app map + history + geofencing.
- **Apple/Google tools** = free passive position while parked in public.

## Open questions
- SIM/eSIM provider that does **both data and SMS** at acceptable cost, with my region's bands.
- Where to host Traccar (VPS vs home), and how to expose it securely.
