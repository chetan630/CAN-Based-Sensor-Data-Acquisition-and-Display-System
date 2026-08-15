# Pin Configuration

## System Overview

The system consists of two LPC1768-based nodes:

- **Transmitter Node** — measures distance using the HC-SR04 ultrasonic sensor and transmits the measured value through CAN.
- **Receiver Node** — receives the distance value through CAN and displays it on an I²C-based LCD.

---

## Transmitter Node

### HC-SR04 Ultrasonic Sensor

| LPC1768 Pin | Function | HC-SR04 Signal |
|---|---|---|
| `P2.11` | GPIO Output | TRIG |
| `P2.12` | GPIO Input | ECHO |

The transmitter generates the trigger signal on `P2.11` and measures the ECHO pulse duration on `P2.12`.

Timer0 is used for timing the ultrasonic ECHO pulse.

### CAN Communication

The transmitter uses the LPC1768 **CAN1 peripheral** to transmit the measured distance to the receiver node.

```text
CAN Identifier: 0x22
```

The measured distance is transmitted as CAN data.

---

## Receiver Node

### CAN Communication

The receiver uses the LPC1768 **CAN1 peripheral** to receive the distance measurement.

The receiver checks for the CAN identifier:

```text
CAN Identifier: 0x22
```

When a matching CAN frame is received, the distance value is extracted from the CAN receive data register.

---

## I²C LCD

The receiver uses **I²C0** to communicate with the LCD.

| LPC1768 Pin | Function | LCD Signal |
|---|---|---|
| `P0.27` | SDA0 | SDA |
| `P0.28` | SCL0 | SCL |

LCD I²C address:

```text
0x27
```

I²C clock frequency:

```text
100 kHz
```

---

## Status LED

The receiver firmware configures:

```text
P1.29
```

as a GPIO output for the status/cross-check LED.

---

## Pin Summary

| Node | Peripheral | LPC1768 Pin | Purpose |
|---|---|---|---|
| Transmitter | GPIO | `P2.11` | HC-SR04 TRIG |
| Transmitter | GPIO | `P2.12` | HC-SR04 ECHO |
| Transmitter | CAN1 | CAN interface | Distance transmission |
| Receiver | CAN1 | CAN interface | Distance reception |
| Receiver | I²C0 | `P0.27` | LCD SDA |
| Receiver | I²C0 | `P0.28` | LCD SCL |
| Receiver | GPIO | `P1.29` | Status LED |

---

## Communication Flow

```text
                  TRANSMITTER NODE
┌─────────────────────────────────────────┐
│                                         │
│              HC-SR04                    │
│                 │                       │
│                 ▼                       │
│             LPC1768 #1                  │
│                 │                       │
│                 │ CAN ID: 0x22          │
└─────────────────┼───────────────────────┘
                  │
                  │ CAN Bus
                  ▼
┌─────────────────────────────────────────┐
│             LPC1768 #2                  │
│                 │                       │
│                 │ I²C0                  │
│                 ▼                       │
│              16×2 LCD                   │
│                                         │
└─────────────────────────────────────────┘
                  RECEIVER NODE
```

---

## Notes

- The transmitter and receiver are implemented as separate LPC1768 firmware projects.
- CAN communication is used for transferring the measured distance between the two controllers.
- The receiver uses I²C0 for LCD communication.
- The documented pin assignments are based on the current project source code.