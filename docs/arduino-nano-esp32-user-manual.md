# Arduino® Nano ESP32 — User Manual

**SKU:** ABX00083  
**Last Modified:** 20/01/2026  
**Source:** [docs.arduino.cc](https://docs.arduino.cc/resources/datasheets/ABX00083-datasheet.pdf)

---

## Description

The Arduino® Nano ESP32 (with and without headers) is a Nano form factor board based on the ESP32-S3 (embedded in the NORA-W106-10B from u-blox®). This is the first Arduino board to be based fully on an ESP32, and features Wi-Fi® as well as Bluetooth® LE.

The Nano ESP32 is compatible with the Arduino Cloud, and has support for MicroPython. It is an ideal board for getting started with IoT development.

**Target areas:** Maker, IoT, MicroPython

---

## Features

### Processor
- Xtensa® Dual-core 32-bit LX7 Microprocessor
- Up to 240 MHz
- 384 kB ROM
- 512 kB SRAM
- 16 kB SRAM in RTC (low power mode)
- DMA Controller

### Power
- Operating voltage: **3.3 V**
- VBUS supplies 5 V via USB-C® connector
- VIN range: **6–21 V**

### Connectivity
- Wi-Fi®
- Bluetooth® LE
- Built-in antenna
- 2.4 GHz transmitter/receiver
- Up to 150 Mbps

### Pins
- 14x digital (21x including analog)
- 8x analog (available in RTC mode)
- SPI (D11, D12, D13), I2C (A4/A5), UART (D0/D1)

### Communication Ports
- SPI, I2C, I2S, UART, CAN (TWAI®)

### Low Power
- **7 μA** consumption in deep sleep mode*
- **240 μA** consumption in light sleep mode*
- RTC Memory
- Ultra Low Power (ULP) Coprocessor
- Power Management Unit (PMU)
- ADC in RTC mode

> *Low power consumption ratings are only for the ESP32-S3 SoC. Other components (e.g. LEDs) increase overall board power consumption.

---

## Recommended Operating Conditions

| Symbol   | Description                        | Min  | Typ  | Max  | Unit |
|----------|------------------------------------|------|------|------|------|
| VIN      | Input voltage from VIN pad         | 6    | 7.0  | 21   | V    |
| VUSB     | Input voltage from USB connector   | 4.8  | 5.0  | 5.5  | V    |
| Tambient | Ambient Temperature                | -40  | 25   | 105  | °C   |

---

## Board Topology

### Component Reference (Front View)

| Ref  | Description                                        |
|------|----------------------------------------------------|
| M1   | NORA-W106-10B (ESP32-S3 SoC)                       |
| J1   | CX90B-16P USB-C® connector                         |
| JP1  | 1x15 analog header                                 |
| JP2  | 1x15 digital header                                |
| U2   | MP2322GQH step-down converter                      |
| U3   | GD25B128EWIGR 128 Mbit (16 MB) ext. flash memory   |
| DL1  | RGB LED                                            |
| DL2  | LED SCK (serial clock)                             |
| DL3  | LED Power (green)                                  |
| D2   | PMEG6020AELRX Schottky Diode                       |
| D3   | PRTR5V0U2X,215 ESD Protection                      |

---

## NORA-W106-10B (Radio Module / MCU)

The Nano ESP32 features the NORA-W106-10B standalone radio module, embedding an ESP32-S3 series SoC and an embedded antenna. The ESP32-S3 is based on an Xtensa® LX7 series microprocessor.

### Xtensa® Dual-Core 32-bit LX7 Microprocessor

Each core can run at up to 240 MHz with 512 kB SRAM. Features:
- 32-bit customized instruction set
- 128-bit data bus
- 32-bit multiplier / divider
- 384 kB ROM, 512 kB SRAM
- 8 kB RTC FAST and RTC SLOW memory (accessible by ULP coprocessor in deep sleep mode)

### Wi-Fi®

Supports Wi-Fi® 4 (IEEE 802.11 b/g/n), output power EIRP up to 10 dBm, max range 500 m.

| Standard  | Max Speed             |
|-----------|-----------------------|
| 802.11b   | 11 Mbit/s             |
| 802.11g   | 54 Mbit/s             |
| 802.11n   | 72 Mbit/s (HT-20) / 150 Mbit/s (HT-40) |

### Bluetooth®

Supports Bluetooth® LE v5.0, output power EIRP up to 10 dBm, data rates up to 2 Mbps. Supports simultaneous scan and advertise, and multiple connections in peripheral/central mode.

### PSRAM

Includes 8 MB embedded PSRAM (Octal SPI).

### Antenna Gain (GFSK modulation)

**Wi-Fi®:**
- Typical conducted output power: 17 dBm
- Typical radiated output power: 20 dBm EIRP
- Conducted sensitivity: -97 dBm

**Bluetooth® Low Energy:**
- Typical conducted output power: 7 dBm
- Typical radiated output power: 10 dBm EIRP
- Conducted sensitivity: -98 dBm

---

## System

### Resets

The ESP32-S3 supports four reset levels:

| Level  | Description                                                         |
|--------|---------------------------------------------------------------------|
| CPU    | Resets CPU0/CPU1 core                                               |
| Core   | Resets digital system, except RTC peripherals                       |
| System | Resets entire digital system, including RTC peripherals             |
| Chip   | Resets the entire chip                                              |

Software reset is supported. For hardware reset, use the onboard reset button (PB1).

### Timers

- 52-bit system timer with 2x 52-bit counters (16 MHz) and 3x comparators
- 4x general-purpose 54-bit timers
- 3x watchdog timers: MWDT0, MWDT1 (main system), RWDT (RTC module)

### Interrupts

All GPIOs can be configured as interrupts via an interrupt matrix. Supported trigger modes:
- `LOW`, `HIGH`, `CHANGE`, `FALLING`, `RISING`

---

## Serial Communication Protocols

### I2C (Inter-Integrated Circuit)

Default pins:
- **A4** — SDA
- **A5** — SCL

> The I2C bus can be reassigned to almost any GPIO. Note: many libraries assume the default A4/A5 assignment.

### I2S (Inter-IC Sound)

Two I2S controllers for audio device communication. No fixed pins — any free GPIO can be used.

**Standard / TDM mode lines:** MCLK, BCLK, WS, DIN/DOUT  
**PDM mode lines:** CLK, DIN/DOUT

### SPI (Serial Peripheral Interface)

Default pins:
- **SCK** — D13
- **CIPO** — D12
- **COPI** — D11
- **CS** — D10

### UART (Universal Asynchronous Receiver/Transmitter)

Default pins:
- **TX** — D1
- **RX** — D0

### TWAI® / CAN (Two Wire Automotive Interface)

Used for CAN/TWAI® protocol communication (common in automotive). No specific pins assigned — any free GPIO can be used.

> TWAI® is also known as CAN2.0B ("CAN classic"). **Not compatible with CAN FD frames.**

---

## External Flash Memory

- **IC:** GD25B128EWIGR (U3)
- **Capacity:** 128 Mbit (16 MB)
- **Interface:** Quad SPI (QSPI)
- **Operating frequency:** 133 MHz
- **Data transfer rate:** up to 664 Mbit/s

---

## USB Connector

One USB-C® port used for:
- Powering the board
- Programming
- Serial communication (send & receive)

> Do not power the board with more than **5 V** via the USB-C® port.

---

## Power Options

Power can be supplied via the **VIN pin** or the **USB-C® connector**. All input is stepped down to **3.3 V** using the MP2322GQH (U2) converter.

> ⚠️ There is **no 5V pin** on the Nano ESP32. 5 V is only available from VBUS when powered via USB.

### VIN Rating

Recommended input voltage: **6–21 V**

| VIN   | Converter Efficiency |
|-------|----------------------|
| 4.5 V | > 90%               |
| 12 V  | 85–90%              |
| 18 V  | < 85%               |

### VBUS

5 V is supplied directly from the USB-C® power source. VBUS is **not activated** when powering via VIN.

### 3.3 V Pin

Connected to the 3.3 V rail (output of MP2322GQH). Used primarily to power external components.

### Pin Voltage & Current

- All digital & analog pins: **3.3 V** — do not connect higher voltage devices
- GPIO source current: up to **40 mA**
- GPIO sink current: up to **28 mA**

---

## Pinout

### Analog Header — JP1 (Left side)

| Pin | Function     | Type   | Description                             |
|-----|--------------|--------|-----------------------------------------|
| 1   | D13 / SCK    | NC     | Serial Clock                            |
| 2   | +3V3         | Power  | +3.3 V Power Rail                       |
| 3   | BOOT0        | Mode   | Board Reset 0                           |
| 4   | A0           | Analog | Analog input 0                          |
| 5   | A1           | Analog | Analog input 1                          |
| 6   | A2           | Analog | Analog input 2                          |
| 7   | A3           | Analog | Analog input 3                          |
| 8   | A4           | Analog | Analog input 4 / I²C SDA               |
| 9   | A5           | Analog | Analog input 5 / I²C SCL               |
| 10  | A6           | Analog | Analog input 6                          |
| 11  | A7           | Analog | Analog input 7                          |
| 12  | VBUS         | Power  | USB power (5 V)                         |
| 13  | BOOT1        | Mode   | Board Reset 1                           |
| 14  | GND          | Power  | Ground                                  |
| 15  | VIN          | Power  | Voltage Input                           |

### Digital Header — JP2 (Right side)

| Pin | Function      | Type    | Description                             |
|-----|---------------|---------|-----------------------------------------|
| 1   | D12 / CIPO*   | Digital | Controller In Peripheral Out            |
| 2   | D11 / COPI*   | Digital | Controller Out Peripheral In            |
| 3   | D10 / CS*     | Digital | Chip Select                             |
| 4   | D9            | Digital | Digital pin 9                           |
| 5   | D8            | Digital | Digital pin 8                           |
| 6   | D7            | Digital | Digital pin 7                           |
| 7   | D6            | Digital | Digital pin 6                           |
| 8   | D5            | Digital | Digital pin 5                           |
| 9   | D4            | Digital | Digital pin 4                           |
| 10  | D3            | Digital | Digital pin 3                           |
| 11  | D2            | Digital | Digital pin 2                           |
| 12  | GND           | Power   | Ground                                  |
| 13  | RST           | Internal| Reset                                   |
| 14  | D0 / RX       | Digital | Serial Receiver (RX)                    |
| 15  | D1 / TX       | Digital | Serial Transmitter (TX)                 |

> *CIPO/COPI/CS replaces the legacy MISO/MOSI/SS terminology.

---

## Board Operation

### Getting Started — Arduino IDE

Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software) and connect the board via a USB-C® cable. The LED (DL1) will indicate power.

Install the **Nano ESP32** board package via the Board Manager:
1. Open Board Manager from the left menu
2. Search for "Nano ESP32"
3. Install the latest version

> The Nano ESP32 uses the Arduino ESP32 Boards core (based on the 2.x branch of arduino-esp32, leveraging ESP-IDF v5.1.4).

### Getting Started — Arduino Cloud Editor

All Arduino boards work out-of-the-box on the [Arduino Cloud Editor](https://create.arduino.cc/editor) with a simple plugin — always up to date.

### Getting Started — Arduino Cloud

Supports logging, graphing, and analyzing sensor data, triggering events, and automating home/business applications.

### Board Recovery

If a sketch locks up the processor and the board is unreachable via USB, **double-tap the reset button** right after power-up to enter bootloader mode.

> This double-tap recovery is provided by the **Arduino bootloader**. If you flash a different
> bootloader (e.g. an ESP-IDF build, as in this project's `firmware/`), double-tap no longer
> works — use the **B1 download-mode** procedure below instead.

#### Firmware download mode via B1 (GPIO0)

The **B1** test pad connects to the ESP32-S3's **GPIO0** strapping pin, with a **GND** pad right
next to it. Pulling GPIO0 low during reset forces the ROM **firmware download mode**, used by
`esptool` / `idf.py flash` (and by Arduino's "Burn Bootloader" to restore the Arduino bootloader).

1. Short **B1 ↔ GND**. The RGB LED turns **green** — this confirms you're on the correct pads
   (a **red** LED means the wrong pads, e.g. a short to 3V3).
2. While still shorted, press and release the white **RST** button.
3. Remove the jumper. The RGB LED stays **purple** (blue/yellow on early board revisions) — the
   board is now in download mode and enumerates a **stable** USB serial COM port.

> ⚠️ Use the **B1** pad, *not* the `BOOT0` entry on the JP1 analog header — they are not the
> same net. Source: [Arduino — Reset the bootloader on the Nano ESP32](https://support.arduino.cc/hc/en-us/articles/9810414060188-Reset-the-Arduino-bootloader-on-the-Nano-ESP32).

---

## Application Examples

- **Home automation:** Smart switches, automatic lighting, motor-controlled blinds
- **IoT sensors:** ADC channels, I2C/SPI buses for sensor monitoring
- **Low power designs:** Battery-powered apps using ESP32-S3 low power modes

---

## Reference Documentation

| Resource                     | Link                                                                 |
|------------------------------|----------------------------------------------------------------------|
| Arduino IDE (Desktop)        | https://www.arduino.cc/en/Main/Software                             |
| Arduino Cloud Editor         | https://create.arduino.cc/editor                                    |
| Cloud Editor - Getting Started | https://docs.arduino.cc/arduino-cloud/guides/editor/              |
| Arduino Project Hub          | https://create.arduino.cc/projecthub?by=part&part_id=11332&sort=trending |
| Library Reference            | https://github.com/arduino-libraries/                               |
| Online Store                 | https://store.arduino.cc/                                           |

---

## Certifications

- **CE DoC (EU):** Conforms to essential requirements of relevant EU Directives
- **RoHS 2 / RoHS 3:** Compliant with EU Directives 2011/65/EU and 2015/863/EU
- **REACH:** No SVHCs present above 0.1% concentration
- **FCC Part 15 Class B:** Compliant; minimum 20 cm distance from body required
- **Industry Canada RSS:** Licence-exempt; minimum 20 cm distance from body required
- **SRRC:** CMIIT ID: 24J993CLD252

### RoHS Substance Limits

| Substance                                    | Max (ppm) |
|----------------------------------------------|-----------|
| Lead (Pb)                                    | 1000      |
| Cadmium (Cd)                                 | 100       |
| Mercury (Hg)                                 | 1000      |
| Hexavalent Chromium (Cr6+)                   | 1000      |
| Poly Brominated Biphenyls (PBB)              | 1000      |
| Poly Brominated Diphenyl ethers (PBDE)       | 1000      |
| Bis(2-Ethylhexyl) phthalate (DEHP)           | 1000      |
| Benzyl butyl phthalate (BBP)                 | 1000      |
| Dibutyl phthalate (DBP)                      | 1000      |
| Diisobutyl phthalate (DIBP)                  | 1000      |

---

## Company Information

| Field           | Details                                        |
|-----------------|------------------------------------------------|
| Company Name    | Arduino S.r.l.                                 |
| Address         | Via Andrea Appiani, 25, Monza, MB, 20900, Italy |

---

## Change Log

| Date       | Changes                                          |
|------------|--------------------------------------------------|
| 08/06/2023 | Initial release                                  |
| 09/01/2023 | Updated power tree flowchart                     |
| 09/11/2023 | Updated SPI section, analog/digital pin section  |
| 11/06/2023 | Corrected company name, corrected VBUS/VUSB      |
| 11/09/2023 | Block diagram update, antenna specifications     |
| 11/15/2023 | Ambient temperature update                       |
| 11/23/2023 | Added labels to LP modes                         |
| 23/02/2024 | Added antenna frequency to block diagram         |
| 25/04/2024 | Updated link to new Cloud Editor                 |
| 23/08/2024 | Added SRRC certification                         |
| 05/09/2024 | Cloud Editor updated from Web Editor             |
