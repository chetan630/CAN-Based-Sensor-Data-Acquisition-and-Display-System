# Test Results

## 1. Test Overview

The CAN-Based Sensor Data Acquisition and Display System was tested as a distributed embedded system consisting of:

- LPC1768 transmitter node
- HC-SR04 ultrasonic sensor
- CAN bus communication
- LPC1768 receiver node
- I²C-based 16×2 LCD

The tests focused on verifying sensor acquisition, CAN communication, data reception, and real-time display.

---

## 2. Functional Tests

| Test | Expected Result | Status |
|---|---|---|
| LPC1768 transmitter initialization | Controller initializes correctly | Tested |
| LPC1768 receiver initialization | Controller initializes correctly | Tested |
| HC-SR04 trigger generation | Ultrasonic measurement is initiated | Tested |
| HC-SR04 echo measurement | Echo pulse duration is measured | Tested |
| Distance calculation | Distance value is calculated | Tested |
| CAN initialization | CAN peripheral initializes correctly | Tested |
| CAN transmission | Distance data is transmitted | Tested |
| CAN reception | Receiver obtains transmitted data | Tested |
| CAN identifier checking | Frame with ID `0x22` is processed | Tested |
| I²C initialization | I²C0 initializes correctly | Tested |
| LCD initialization | LCD becomes operational | Tested |
| Distance display | Received distance is displayed | Tested |
| End-to-end operation | Sensor → CAN → LCD operation | Tested |

---

## 3. HC-SR04 Measurement Test

The HC-SR04 was connected to the transmitter LPC1768.

```text
TRIG → P2.11
ECHO → P2.12
```

Timer0 was used to measure the ECHO pulse duration.

### Test Objective

Verify that the transmitter can:

1. Generate the ultrasonic trigger signal.
2. Receive the ECHO signal.
3. Measure the ECHO pulse duration.
4. Calculate the corresponding distance.

### Result

The ultrasonic distance measurement functionality was implemented and tested as part of the transmitter firmware.

---

## 4. CAN Communication Test

The transmitter and receiver LPC1768 controllers communicate through the CAN bus.

The transmitter sends the distance measurement using:

```text
CAN ID: 0x22
```

The receiver checks for the same CAN identifier and extracts the received distance value.

### Test Objective

Verify that:

- CAN1 initializes correctly.
- The transmitter can send a CAN frame.
- The receiver can receive the CAN frame.
- The receiver identifies CAN ID `0x22`.
- The transmitted distance value is correctly extracted.

### Result

CAN-based data transmission and reception were implemented and tested between the two LPC1768 nodes.

---

## 5. I²C LCD Test

The receiver uses I²C0 to communicate with the LCD.

```text
SDA → P0.27
SCL → P0.28
LCD Address → 0x27
I²C Frequency → 100 kHz
```

### Test Objective

Verify that the receiver can:

- Initialize I²C0.
- Communicate with the LCD.
- Receive distance data through CAN.
- Display the received distance on the LCD.

### Result

The received distance measurement was displayed on the I²C-based LCD.

---

## 6. End-to-End System Test

The complete system was tested using the following sequence:

```text
HC-SR04
   │
   ▼
Distance Measurement
   │
   ▼
LPC1768 Transmitter
   │
   ▼
CAN Transmission
   │
   ▼
LPC1768 Receiver
   │
   ▼
CAN Data Reception
   │
   ▼
I²C LCD
   │
   ▼
Distance Display
```

### Result

The complete sensor-to-display data path was implemented:

**HC-SR04 → LPC1768 → CAN → LPC1768 → I²C LCD**

---

## 7. Performance Results

The project description reports the following target/observed performance values:

| Parameter | Reported Result |
|---|---:|
| Distance measurement accuracy | ~3 mm |
| Maximum tested range | ~4 m |
| CAN data transfer latency | <150 ms |
| I²C clock frequency | 100 kHz |

These values should be interpreted as project test results rather than guaranteed specifications of the individual components.

---

## 8. Test Summary

The main system functions were successfully implemented and tested:

- Ultrasonic distance acquisition
- Timer-based echo measurement
- CAN data transmission
- CAN data reception
- CAN identifier handling
- I²C LCD communication
- Real-time distance display
- End-to-end sensor-to-display operation

---

## 9. Development Environment

| Parameter | Configuration |
|---|---|
| Microcontroller | LPC1768 |
| Core | ARM Cortex-M3 |
| Sensor | HC-SR04 |
| Communication | CAN |
| Display Interface | I²C |
| LCD | 16×2 LCD |
| Programming Language | Embedded C |
| Development IDE | Keil µVision 4 |