# Apple Find My viewer (Macless-Haystack endpoint) setup

How to stand up the **Macless-Haystack** endpoint + frontend so the Phase 1a beacon's
location (reported by passing iPhones) can be read back over home internet — no Mac, no SIM.
Pairs with [`../firmware/ble-beacon/README.md`](../firmware/ble-beacon/README.md) and
[`component-ble-crowdsourced.md`](component-ble-crowdsourced.md). Upstream lives in
`external/macless-haystack/` (cloned; v2.7.2 at time of writing).

## Architecture (who holds what)
- **Device** advertises rolling *public* (advertisement) keys. Passing iPhones upload encrypted
  location reports to Apple.
- **Endpoint** (Docker) authenticates to Apple with *your* Apple ID (via an **anisette** server)
  and fetches the encrypted reports for given hashed keys.
- **Frontend** (web or Android app) holds the device **private** keys (`*_devices.json`),
  asks the endpoint for reports, and decrypts them locally to show a map pin.

## Prerequisites
- **Docker Desktop** installed and **running**.
- An **Apple ID** with **2FA via SMS/text** (authenticator-app TOTP is not supported). Use a
  **dedicated** account — but note the trust-score gate below: a *brand-new* account often fails.
- Keys already generated (`output/tracker01_devices.json`, see firmware runbook).

## Server setup (normal PowerShell, not the ESP-IDF shell)
```powershell
# 1. Network
docker network create mh-network

# 2. Anisette server (Apple auth tokens)
docker run -d --restart always --name anisette -p 6969:6969 `
  --volume anisette-v3_data:/home/Alcoholic/.config/anisette-v3 `
  --network mh-network dadoum/anisette-v3-server

# 3. Endpoint — interactive first run; enter Apple ID, password, SMS code
docker run -it --restart unless-stopped --name macless-haystack -p 6176:6176 `
  --volume mh_data:/app/endpoint/data --network mh-network christld/macless-haystack
#    success == "serving at port 6176 over HTTP"

# 4. Re-run detached
docker restart macless-haystack
```

## Frontend
- Web UI: <https://dchristl.github.io/macless-haystack/> → Settings → endpoint URL
  `http://localhost:6176` → **import** `output/tracker01_devices.json`.
- The page is HTTPS and the endpoint is HTTP on localhost; Chrome usually allows that
  (localhost is "potentially trustworthy"). If blocked, allow insecure content for the site,
  or use the **Android app** (same import). SSL notes in `external/macless-haystack/FAQ.md`.

## Expectations
- Reports are **not instant** — first fix often **15–60+ min**, and only after the beacon has
  been **near an iPhone** with Find My network enabled. Continuous advertising
  (`BEACON_NO_DEEP_SLEEP 1`) helps it be heard sooner. If you have an iPhone, leave it nearby
  to seed reports.

---

## ⚠️ The Apple account trust-score gate (main blocker)

Symptom during endpoint login (HTTP 200, 2FA may even be skipped on retries):
```
com.apple.mobileme -> status 1, "A server problem is blocking Apple ID sign in. Try signing in later."
ERROR: It seems your account score is not high enough...
```
This is **Apple-side**, not a setup bug — the rest of the stack worked. A registered fake
device showing as *"MacBookPro 14 / serial 0 / Find My Mac off / not trusted"* is normal (the
login spoofs a Mac); it is not the problem.

**Fixes, most effective first:**
1. **Sign into this Apple ID on a real Apple device once** (iPhone/iPad/Mac), with iCloud +
   Find My on. The strongest signal; usually clears the gate.
2. **Add a payment method (credit card) + profile data** at <https://appleid.apple.com/> and
   **wait** — the score can take hours–a day to update. (Adding a card + address alone did
   *not* immediately clear it in our testing on 2026-06-27.)
3. **Use an *established* Apple ID** (real device/purchase history) instead of a fresh one.
4. **Apple Music route** (FAQ): accounts set up/aged via the Apple Music Android app reportedly
   avoid this.
5. **Fallback login:** generate an `auth.json` with [biemster/FindMy](https://github.com/biemster/FindMy),
   drop it in the `mh_data` volume, restart — skips macless-haystack's own registration.

**Retry registration after changes:**
```powershell
docker stop macless-haystack
docker start -ai macless-haystack
```

**Gotchas on retries:**
- Each login attempt registers a new "device" on the account. Too many breaks login — prune at
  <https://account.apple.com/account/manage/section/devices>.
- **Switching Apple ID:** clear cached creds first — `docker rm -f macless-haystack;
  docker volume rm mh_data` — then re-run the interactive endpoint.

## Quick health checks
- `docker ps` — both `anisette` and `macless-haystack` running.
- `docker logs macless-haystack` — authenticated and `serving at port 6176`.
- `idf.py -p COM<N> monitor` — beacon shows `advertising has started` and is within BLE range
  of an iPhone.
