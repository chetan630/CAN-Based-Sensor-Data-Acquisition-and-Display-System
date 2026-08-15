# Project Documentation

## 1. Overview

The **CAN-Based Sensor Data Acquisition and Display System** is a distributed embedded system developed using two LPC1768 ARM Cortex-M3 microcontrollers.

The system consists of:

- A **transmitter node** for ultrasonic distance measurement.
- A **receiver node** for CAN data reception and LCD display.
- An **HC-SR04 ultrasonic sensor** connected to the transmitter.
- A **CAN bus** connecting the two LPC1768 controllers.
- An **I²C-based 16×2 LCD** connected to the receiver.

The firmware is implemented using register-level programming for the LPC1768 peripherals.

---

## 2. System Architecture

```text
                    TRANSMITTER NODE
             ┌──────────────────────────┐
             │       LPC1768 #1         │
             │                          │
             │     Timer0 / GPIO        │
             └────────────┬─────────────┘
                          │
                          │
                    HC-SR04 Sensor
                          │
                          ▼
                   Distance Measurement
                          │
                          ▼
                       CAN1
                          │
                          │ CAN Bus
                          ▼
                       CAN1
             ┌──────────────────────────┐
             │       LPC1768 #2         │
             │                          │
             │     CAN Reception        │
             │           │              │
             │           ▼              │
             │         I²C0             │
             └───────────┬──────────────┘
                         │
                         ▼
                    16×2 LCD
                         │
                         ▼
                  Distance Display
                    RECEIVER NODE
```

---

## 3. Transmitter Node

The transmitter LPC1768 is responsible for acquiring distance information from the HC-SR04 ultrasonic sensor.

### Main Functions

1. Initialize the required LPC1768 peripherals.
2. Configure the HC-SR04 trigger and echo signals.
3. Generate the ultrasonic trigger pulse.
4. Measure the ECHO pulse duration using Timer0.
5. Calculate the corresponding distance.
6. Initialize the CAN1 peripheral.
7. Transmit the distance measurement through CAN.

---

## 4. HC-SR04 Distance Measurement

The HC-SR04 uses a trigger pulse to start an ultrasonic measurement.

The transmitter firmware controls:

```text
TRIG → P2.11
ECHO → P2.12
```

The trigger signal is generated using GPIO.

The ECHO signal is monitored to determine the duration of the ultrasonic return pulse.

Timer0 is used to measure the ECHO pulse duration.

The measured timing information is then converted into a distance value.

### Measurement Flow

```text
Generate Trigger Pulse
        │
        ▼
HC-SR04 Sends Ultrasonic Pulse
        │
        ▼
Echo Signal Received
        │
        ▼
Measure Echo Pulse Duration
        │
        ▼
Calculate Distance
        │
        ▼
Transmit Distance through CAN
```

---

## 5. CAN Communication

The two LPC1768 microcontrollers communicate using the LPC1768 **CAN1 peripheral**.

The transmitter sends the measured distance using:

```text
CAN ID: 0x22
```

The receiver monitors the received CAN frames and checks for the same identifier.

### CAN Data Flow

```text
HC-SR04
   │
   ▼
LPC1768 Transmitter
   │
   │ CAN ID: 0x22
   │ Distance Data
   ▼
CAN Bus
   │
   ▼
LPC1768 Receiver
   │
   ▼
LCD Display
```

---

## 6. Receiver Node

The receiver LPC1768 receives the distance measurement transmitted by the transmitter node.

### Main Functions

1. Initialize the CAN1 peripheral.
2. Configure CAN reception.
3. Monitor incoming CAN frames.
4. Check the received CAN identifier.
5. Extract the distance value.
6. Initialize the I²C0 interface.
7. Update the LCD with the received distance.

The receiver uses a CAN interrupt handler to process received CAN data.

---

## 7. CAN Reception

The receiver checks the received CAN identifier:

```text
CAN ID: 0x22
```

When a matching frame is received, the corresponding distance data is extracted from the CAN receive data register.

The received value is then passed to the LCD display routine.

---

## 8. I²C LCD Interface

The receiver uses the LPC1768 **I²C0 peripheral** to communicate with the LCD.

The interface uses:

```text
SDA → P0.27
SCL → P0.28
```

The LCD address used by the firmware is:

```text
0x27
```

The I²C interface is configured for:

```text
100 kHz
```

### LCD Data Flow

```text
CAN Frame
    │
    ▼
Received Distance
    │
    ▼
LPC1768 Receiver
    │
    ▼
I²C0
    │
    ▼
16×2 LCD
    │
    ▼
Distance Display
```

---

## 9. Firmware Organization

The project contains separate firmware implementations for the transmitter and receiver.

### Transmitter

```text
ultra_sonic_CAN_tx.c
│
├── GPIO Configuration
├── Timer0 Configuration
├── HC-SR04 Control
├── Distance Measurement
├── CAN1 Configuration
├── CAN Transmission
└── Main Application
```

### Receiver

```text
ultra_sonic_CAN_rx.c
│
├── CAN1 Configuration
├── CAN Reception
├── CAN Interrupt Handler
├── I²C0 Configuration
├── LCD Functions
├── Distance Display
└── Main Application
```

---

## 10. Communication Sequence

The complete system operates as follows:

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
HC-SR04 Measurement
   │
   ▼
Distance Calculation
   │
   ▼
CAN Transmission
   │
   ▼
CAN Reception
   │
   ▼
CAN ID Verification
   │
   ▼
Distance Extraction
   │
   ▼
I²C LCD Update
   │
   ▼
Distance Display
   │
   └──────────────► Repeat
```

---

## 11. Development Approach

The firmware uses direct LPC1768 peripheral register configuration rather than relying on a high-level hardware abstraction library.

The implementation uses LPC1768 peripherals for:

- GPIO
- Timer0
- CAN1
- I²C0

The transmitter and receiver are maintained as separate firmware projects.

---

## 12. Keil Project Organization

The project contains separate Keil projects for the two nodes:

```text
keil/
│
├── transmitter/
│   └── can-transmitter.uvproj
│
└── receiver/
    └── can-receiver.uvproj
```

The transmitter project contains the ultrasonic measurement and CAN transmission firmware.

The receiver project contains the CAN reception and LCD display firmware.

---

## 13. Hardware Communication Flow

```text
       HC-SR04
          │
          │ Distance
          ▼
   ┌──────────────┐
   │  LPC1768 #1  │
   │ Transmitter  │
   └──────┬───────┘
          │
          │ CAN
          │ ID 0x22
          ▼
   ┌──────────────┐
   │  LPC1768 #2  │
   │   Receiver   │
   └──────┬───────┘
          │
          │ I²C
          ▼
      16×2 LCD
          │
          ▼
   Distance Display
```

---

## 14. Project Limitations

The current implementation is a prototype consisting of separate transmitter and receiver firmware.

The distance value is transmitted through the configured CAN frame and displayed on the receiver LCD.

Detailed measured performance results should be referred to in the project's test documentation.

---

## 15. Future Improvements

Possible improvements include:

- More robust CAN error handling.
- CAN bus fault monitoring.
- Improved ultrasonic measurement filtering.
- Averaging multiple distance measurements.
- Improved LCD update handling.
- Modular separation of peripheral drivers.
- Watchdog-based fault recovery.
- Extended CAN data packet structure for additional sensors.