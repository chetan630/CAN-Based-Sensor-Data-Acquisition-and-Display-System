# CAN-Based Sensor Data Acquisition and Display System

**LPC1768 | ARM Cortex-M3 | Embedded C | CAN | I²C | HC-SR04**

A distributed embedded firmware system using two LPC1768 ARM Cortex-M3 microcontrollers for ultrasonic distance measurement, CAN-based data communication, and real-time LCD display.

The transmitter node acquires distance data from an HC-SR04 ultrasonic sensor and transmits the measurement through CAN. The receiver node receives the CAN data and displays the distance on a 16×2 I²C LCD.

The firmware uses register-level peripheral programming and was developed using Keil µVision 4.

---

## Features

- Dual LPC1768 microcontroller architecture
- HC-SR04 ultrasonic distance measurement
- Timer0-based echo pulse measurement
- CAN1-based communication between LPC1768 nodes
- CAN identifier `0x22` for distance data
- Interrupt-driven CAN reception
- I²C0-based 16×2 LCD interface
- LCD I²C address `0x27`
- 100 kHz I²C communication
- Register-level peripheral programming
- Real-time distance display

---

## System Architecture

```text
                 TRANSMITTER NODE

        ┌──────────────────────┐
        │      HC-SR04         │
        │ Ultrasonic Sensor    │
        └──────────┬───────────┘
                   │
                   │ Distance
                   ▼
        ┌──────────────────────┐
        │      LPC1768 #1      │
        │     Transmitter      │
        │                      │
        │ GPIO + Timer0        │
        │ CAN1                 │
        └──────────┬───────────┘
                   │
                   │ CAN
                   │ ID: 0x22
                   ▼
              ───────────
                CAN BUS
              ───────────
                   │
                   ▼

        ┌──────────────────────┐
        │      LPC1768 #2      │
        │      Receiver        │
        │                      │
        │ CAN1 + I²C0          │
        └──────────┬───────────┘
                   │
                   │ I²C
                   ▼
        ┌──────────────────────┐
        │       16×2 LCD       │
        │                      │
        │ Distance Display     │
        └──────────────────────┘
```

---

## Working Principle

The system operates in the following sequence:

```text
Power On
   │
   ▼
Initialize Transmitter
   │
   ├── GPIO
   ├── Timer0
   └── CAN1
   │
   ▼
Initialize Receiver
   │
   ├── CAN1
   └── I²C0 LCD
   │
   ▼
Trigger HC-SR04
   │
   ▼
Measure Echo Pulse
   │
   ▼
Calculate Distance
   │
   ▼
Transmit CAN Frame
   │
   ▼
Receive CAN Frame
   │
   ▼
Check CAN ID 0x22
   │
   ▼
Extract Distance
   │
   ▼
Update LCD through I²C
   │
   ▼
Display Distance
   │
   └──────────────► Repeat
```

---

## Hardware

| Component | Purpose | Interface |
|---|---|---|
| LPC1768 #1 | Distance acquisition and CAN transmission | GPIO, Timer0, CAN |
| LPC1768 #2 | CAN reception and display control | CAN, I²C |
| HC-SR04 | Ultrasonic distance measurement | GPIO |
| 16×2 LCD | Distance display | I²C |
| CAN Bus | Communication between controllers | CAN |

---

## Pin Configuration

### Transmitter Node

| LPC1768 Pin | Function | Connected Device |
|---|---|---|
| `P2.11` | GPIO Output | HC-SR04 TRIG |
| `P2.12` | GPIO Input | HC-SR04 ECHO |

### Receiver Node

| LPC1768 Pin | Function | Connected Device |
|---|---|---|
| `P0.27` | SDA0 | LCD SDA |
| `P0.28` | SCL0 | LCD SCL |
| `P1.29` | GPIO Output | Status LED |

### CAN

The transmitter and receiver communicate using the LPC1768 CAN1 peripheral.

```text
CAN Identifier: 0x22
```

For the complete pin configuration, see:

[`docs/pin-configuration.md`](docs/pin-configuration.md)

---

## Communication Interfaces

### HC-SR04

The transmitter controls the HC-SR04 using GPIO:

```text
TRIG → P2.11
ECHO → P2.12
```

Timer0 is used to measure the ECHO pulse duration and determine the distance.

---

### CAN

CAN1 is used for communication between the two LPC1768 controllers.

The transmitter sends the measured distance using:

```text
CAN ID: 0x22
```

The receiver checks the received identifier and extracts the corresponding distance data.

---

### I²C LCD

The receiver uses I²C0 to communicate with the LCD.

```text
SDA → P0.27
SCL → P0.28
LCD Address → 0x27
I²C Frequency → 100 kHz
```

---

## Firmware Structure

### Transmitter

```text
src/
└── ultra_sonic_CAN_tx.c
```

Main functions include:

- GPIO configuration
- Timer0 configuration
- HC-SR04 control
- Distance measurement
- CAN1 initialization
- CAN data transmission

### Receiver

```text
src/
├── ultra_sonic_CAN_rx.c
└── ultra_sonic_CAN_LCD_rx.c
```

The receiver firmware handles:

- CAN1 initialization
- CAN frame reception
- CAN interrupt handling
- Distance extraction
- I²C0 initialization
- LCD communication
- Distance display

---

## Repository Structure

```text
CAN-Based-Sensor-Data-Acquisition-and-Display-System/
│
├── README.md
├── .gitignore
│
├── src/
│   ├── ultra_sonic_CAN_tx.c
│   ├── ultra_sonic_CAN_rx.c
│   ├── ultra_sonic_CAN_LCD_rx.c
│   ├── startup_LPC17xx.s
│   └── system_LPC17xx.c
│
├── keil/
│   ├── transmitter/
│   │   ├── can-transmitter.uvproj
│   │   ├── can-transmitter.uvopt
│   │   └── ...
│   │
│   └── receiver/
│       ├── can-receiver.uvproj
│       ├── can-receiver.uvopt
│       └── ...
│
├── docs/
│   ├── pin-configuration.md
│   ├── project-documentation.md
│   └── test-results.md
│
├── datasheets/
│
└── Images/
```

---

## Keil µVision 4

The firmware was developed using **Keil µVision 4**.

Separate Keil projects are maintained for the two LPC1768 nodes:

```text
keil/
├── transmitter/
│   └── can-transmitter.uvproj
│
└── receiver/
    └── can-receiver.uvproj
```

### Building the Transmitter

1. Open `can-transmitter.uvproj` in Keil µVision 4.
2. Select the appropriate target.
3. Build the project.
4. Program the transmitter LPC1768 using the available debug/programming interface.

### Building the Receiver

1. Open `can-receiver.uvproj` in Keil µVision 4.
2. Select the appropriate target.
3. Build the project.
4. Program the receiver LPC1768.

Generated build files are excluded from version control using `.gitignore`.

---

## Testing

The system was tested for:

- LPC1768 peripheral initialization
- HC-SR04 trigger and echo operation
- Distance measurement
- CAN transmission
- CAN reception
- CAN identifier handling
- I²C communication
- LCD initialization
- Real-time distance display
- End-to-end sensor-to-display operation

Reported project performance:

| Parameter | Result |
|---|---:|
| Distance measurement accuracy | ~3 mm |
| Tested distance range | Up to ~4 m |
| CAN data transfer latency | <150 ms |
| I²C clock frequency | 100 kHz |

Detailed test information is available in:

[`docs/test-results.md`](docs/test-results.md)

---

## Documentation

| Document | Description |
|---|---|
| [`Pin Configuration`](docs/pin-configuration.md) | LPC1768 pin assignments and module connections |
| [`Project Documentation`](docs/project-documentation.md) | Detailed system operation and firmware architecture |
| [`Test Results`](docs/test-results.md) | Functional and performance testing |

---

## Development Environment

| Category | Technology |
|---|---|
| Microcontroller | LPC1768 |
| CPU | ARM Cortex-M3 |
| Programming Language | Embedded C |
| IDE | Keil µVision 4 |
| Sensor | HC-SR04 |
| Communication | CAN |
| Display Interface | I²C |
| Display | 16×2 LCD |
| Programming Approach | Register-level programming |

---

## Limitations

- The current implementation uses separate transmitter and receiver firmware.
- The HC-SR04 measurement is dependent on the sensor's operating conditions.
- The current CAN implementation is designed around the project-specific distance data frame.
- The LCD display is limited to the implemented 16×2 interface.
- The project is a prototype implementation.

---

## Future Improvements

- Add CAN error and bus fault handling.
- Implement distance measurement filtering.
- Average multiple sensor readings for improved stability.
- Add additional sensor data to the CAN frame.
- Improve modularity by separating peripheral drivers into `.c` and `.h` files.
- Add watchdog-based fault recovery.
- Improve diagnostic and error reporting.
- Develop a dedicated PCB for a production-oriented version.

---

## Author

**Chetan Angadi**

Electronics and Communication Engineering

GitHub: [chetan630](https://github.com/chetan630)