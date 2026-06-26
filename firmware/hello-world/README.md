# Phase 0 — hello-world (Arduino Nano ESP32 / ESP32-S3)

Minimal ESP-IDF project that proves the toolchain end-to-end: it boots, prints chip
info, and logs a heartbeat with a running uptime over the board's native USB console.
It also ships the **host-side logic unit-test pipeline** later phases build on.

**Phase 0 is done when** `idf.py -p <PORT> flash monitor` shows the boot banner and a
heartbeat ticking, *and* the host unit tests pass on the PC.

> Board: **Arduino Nano ESP32** (ABX00083) — ESP32-S3 in a u-blox NORA-W106 module,
> native USB. Datasheet: [`../../docs/arduino-nano-esp32-user-manual.md`](../../docs/arduino-nano-esp32-user-manual.md).

---

## Layout

```
hello-world/
  CMakeLists.txt              # project
  sdkconfig.defaults          # console over USB-Serial/JTAG (native USB)
  main/hello_world_main.c     # app_main: chip info + heartbeat
  components/banner/          # pure-logic helper (host-testable, no HW/RTOS deps)
  host_test/                  # Linux-target Unity tests for components/banner
```

---

## 1. Install ESP-IDF (Windows, one time)

Use the official **ESP-IDF Windows Installer** (recommended — it provisions its own
Python + the esp32s3 toolchain, independent of the system Python 3.14):

1. Download the offline installer from
   <https://dl.espressif.com/dl/esp-idf/> (pick a stable release, e.g. the latest v5.x).
2. Run it; accept the bundled Python, Git, and the ESP-IDF tools.
3. It installs an **"ESP-IDF PowerShell"** / **"ESP-IDF CMD"** Start-menu shortcut that
   opens a shell with the environment exported. Use that shell for all `idf.py` commands.

Alternative (manual clone):
```powershell
git clone -b v5.3.1 --recursive https://github.com/espressif/esp-idf.git $HOME\esp\esp-idf
$HOME\esp\esp-idf\install.ps1 esp32s3
# then in each new shell, activate with:
. $HOME\esp\esp-idf\export.ps1
```

Verify: `idf.py --version` prints a version.

---

## 2. Build + flash + monitor (needs the board)

From an ESP-IDF shell, in this directory:

```powershell
idf.py set-target esp32s3
idf.py build
idf.py -p COM<N> flash monitor      # e.g. -p COM5 ; Ctrl-] to quit the monitor
```

Finding the port: plug in the Nano ESP32 over USB-C and check **Device Manager →
Ports (COM & LPT)** for the new COM number.

**If flashing can't connect** (esptool can't auto-enter the bootloader), put the board in
download mode manually: **double-tap the RESET button**, then re-run `idf.py -p COM<N> flash`.

> Note: flashing ESP-IDF **replaces the factory Arduino bootloader**. That's expected for
> this dedicated ESP-IDF build; you can always re-flash an Arduino sketch later to restore it.

Expected monitor output (abridged):
```
I (xxx) tracker: Motorcycle tracker - Phase 0 hello-world
I (xxx) tracker: Board: Arduino Nano ESP32 (ESP32-S3 / u-blox NORA-W106)
I (xxx) tracker: Chip: esp32s3, 2 core(s), silicon rev v0.2
I (xxx) tracker: Flash: 16 MB, free heap: ...
I (xxx) tracker: alive - uptime 0d 00:00:00.0xx
I (xxx) tracker: alive - uptime 0d 00:00:02.0xx
...
```

---

## 3. Run the host unit tests (no hardware)

The pure logic in `components/banner` is tested on the host (no board, no ESP-IDF "linux"
target — that doesn't build on native Windows). Tests live in [`../test`](../test) and build
with CMake + a host C compiler:

```powershell
cmake -S firmware/test -B firmware/test/build -G "MinGW Makefiles"
cmake --build firmware/test/build
ctest --test-dir firmware/test/build --output-on-failure
```

(Use any generator your host supports; `MinGW Makefiles` matches the bundled msys2 gcc.)

> Scope: **logic unit tests only** for now. Hardware-in-the-loop / on-target testing is a
> separate, later phase.
