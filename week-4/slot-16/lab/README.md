# LAB 16 — Embedded Security: AES-256, SHA-256 Firmware Verification & Secure OTA Update

## 1. Objective

The objective of this lab is to:

1. Understand fundamental security requirements of connected embedded systems.
2. Encrypt and decrypt application data using **AES-256**.
3. Understand symmetric-key encryption and key management.
4. Compute a **SHA-256 cryptographic hash** for data and firmware images.
5. Verify firmware integrity by comparing expected and calculated SHA-256 hashes.
6. Understand the difference between encryption, integrity verification, and authentication.
7. Download a firmware image securely using HTTPS.
8. Verify the downloaded firmware before installation.
9. Perform a basic **Over-the-Air (OTA) firmware update** on the ESP32.
10. Reject corrupted or unexpected firmware.
11. Understand secure boot, signed firmware, and Root of Trust concepts.
12. Construct a basic secure embedded-device update pipeline.

---

## 2. Background

Connected embedded systems may process sensitive information and receive firmware updates through communication networks. Without suitable protection, an attacker may attempt to:

- Read transmitted information
- Modify sensor or control data
- Replace firmware
- Install unauthorized code
- Intercept network traffic
- Replay previously transmitted messages

A secure embedded system therefore requires several complementary security mechanisms:

```text
Confidentiality
      +
Integrity
      +
Authenticity
      +
Secure Firmware Update
```

This lab introduces three important mechanisms:

```text
AES-256
   ↓
Data Confidentiality

SHA-256
   ↓
Data / Firmware Integrity

HTTPS + Firmware Verification + OTA
   ↓
Safer Firmware Distribution
```

---

## 3. Embedded Security Architecture

The laboratory architecture is:

```text
                     ESP32
                       │
       ┌───────────────┼────────────────┐
       │               │                │
       ▼               ▼                ▼
    AES-256          SHA-256           OTA
       │               │                │
       ▼               ▼                ▼
Encryption      Integrity Check    Firmware Update
       │               │                │
       └───────────────┴────────────────┘
                       ↓
              Secure Embedded System
```

The complete security workflow is:

```text
Sensor / Application Data
        ↓
AES-256 Encryption
        ↓
Protected Communication

Firmware Image
        ↓
SHA-256 Verification
        ↓
HTTPS Download
        ↓
OTA Installation
        ↓
Restart
        ↓
Updated Firmware
```

---

## 4. Security Properties

Three security properties should be distinguished.

| Property | Purpose | Example Mechanism |
|---|---|---|
| Confidentiality | Prevent unauthorized reading | AES-256 |
| Integrity | Detect modification | SHA-256 |
| Authenticity | Verify trusted origin | Digital signature / authenticated protocol |

AES protects confidentiality.

SHA-256 can detect whether data changed, but **a plain SHA-256 hash alone does not prove who created the firmware**. A production secure-update system should additionally use digital signatures, authenticated metadata, or another trusted authenticity mechanism.

---

## 5. Hardware & Software Requirements

### Hardware

- ESP32 development board
- USB cable
- Computer
- Wi-Fi network
- Optional LED for update-status indication

### Software

- Arduino IDE or PlatformIO
- ESP32 Arduino Core
- Serial Monitor
- Web server or firmware hosting service

### Libraries / Components

Depending on the ESP32 software environment:

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Update.h>
```

For cryptographic operations, ESP32 environments commonly provide cryptographic functionality through **mbedTLS**.

> API details can vary between ESP32 Arduino Core versions. The examples below are intended as an instructional framework and should be adapted to the installed core and library versions.

---

# 6. Part A — AES-256 Data Encryption

## 6.1 AES

AES stands for:

```text
Advanced Encryption Standard
```

It is a symmetric-key encryption algorithm.

```text
Plaintext
    ↓
AES Encryption
    ↓
Ciphertext
    ↓
AES Decryption
    ↓
Plaintext
```

The same secret key is used for encryption and decryption.

---

## 7. AES Key Sizes

AES supports:

| Version | Key Size |
|---|---:|
| AES-128 | 128 bits |
| AES-192 | 192 bits |
| AES-256 | 256 bits |

This lab uses:

$$
K=256~\text{bits}.
$$

Since:

$$
8~\text{bits}=1~\text{byte},
$$

the AES-256 key length is:

$$
\frac{256}{8}=32~\text{bytes}.
$$

---

## 8. Experiment 1 — Define an AES-256 Key

For a controlled lab demonstration, define a 32-byte key.

```cpp
unsigned char aesKey[32] = {
  0x10, 0x21, 0x32, 0x43,
  0x54, 0x65, 0x76, 0x87,
  0x98, 0xA9, 0xBA, 0xCB,
  0xDC, 0xED, 0xFE, 0x0F,

  0x01, 0x12, 0x23, 0x34,
  0x45, 0x56, 0x67, 0x78,
  0x89, 0x9A, 0xAB, 0xBC,
  0xCD, 0xDE, 0xEF, 0xF0
};
```

This key is intentionally visible for laboratory learning.

For real systems, do **not** hard-code reusable production secrets directly in source code.

---

## 9. Initialization Vector

Many AES modes require an **Initialization Vector (IV)**.

For AES with a 128-bit block size:

$$
IV=128~\text{bits}=16~\text{bytes}.
$$

Example:

```cpp
unsigned char iv[16] = {
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};
```

For real deployments, IV or nonce requirements depend on the selected AES mode. They should not simply be reused across messages when the mode requires uniqueness or unpredictability.

---

## 10. AES Operating Modes

AES operates on fixed-size blocks and therefore requires an operating mode.

Examples include:

```text
ECB
CBC
CTR
GCM
```

For modern systems, authenticated modes such as **AES-GCM** are generally preferable because they can provide both confidentiality and integrity/authentication of encrypted data.

For teaching the basic encryption process, CBC or another supported mode may be demonstrated.

---

## 11. Experiment 2 — Prepare Plaintext

Example application data:

```cpp
const char* plaintext =
    "temperature=27.5";
```

Conceptually:

```text
temperature=27.5
       ↓
AES-256
       ↓
Binary Ciphertext
```

Ciphertext generally cannot be printed directly as readable text.

It may be represented as hexadecimal:

```text
83 2A 91 7F ...
```

---

## 12. Experiment 3 — AES-256 Encryption

A generic cryptographic pipeline is:

```text
Initialize AES Context
        ↓
Load 256-bit Key
        ↓
Prepare IV / Nonce
        ↓
Encrypt Plaintext
        ↓
Generate Ciphertext
```

Conceptual mbedTLS-style code may resemble:

```cpp
#include "mbedtls/aes.h"

mbedtls_aes_context aes;

mbedtls_aes_init(&aes);

mbedtls_aes_setkey_enc(
    &aes,
    aesKey,
    256
);
```

The exact encryption function depends on the selected AES mode.

---

## 13. Experiment 4 — Display Ciphertext

A helper function can print binary data as hexadecimal:

```cpp
void printHex(
    const unsigned char* data,
    size_t length
) {

  for (size_t i = 0; i < length; i++) {

    if (data[i] < 16) {
      Serial.print("0");
    }

    Serial.print(
      data[i],
      HEX
    );

    Serial.print(" ");
  }

  Serial.println();
}
```

Expected concept:

```text
Plaintext:
temperature=27.5

Ciphertext:
A4 73 19 D2 8F ...
```

The ciphertext should appear unrelated to the original plaintext.

---

## 14. Experiment 5 — AES Decryption

The reverse operation is:

```text
Ciphertext
     ↓
AES-256 Decryption
     ↓
Recovered Plaintext
```

The expected result is:

```text
Original:
temperature=27.5

Recovered:
temperature=27.5
```

Successful recovery demonstrates:

$$
D_K(E_K(P))=P,
$$

where:

- $P$ = plaintext
- $E_K$ = encryption using key $K$
- $D_K$ = decryption using key $K$

---

## 15. Experiment 6 — Encrypt Sensor Data

Create a simple sensor message:

```cpp
float temperature = 27.5;
float humidity = 68.0;
```

Convert to an application message:

```text
temperature=27.5,humidity=68.0
```

Then:

```text
Sensor
   ↓
Application Message
   ↓
AES-256
   ↓
Encrypted Payload
```

This illustrates confidential telemetry.

---

## 16. AES Experiment Result Table

Record:

| Test | Plaintext | Ciphertext Length | Decryption Correct |
|---:|---|---:|---|
| 1 | Hello ESP32 |  |  |
| 2 | temperature=27.5 |  |  |
| 3 | humidity=68 |  |  |
| 4 | sensor=2048 |  |  |

---

# 17. Part B — SHA-256 Integrity Verification

SHA-256 stands for:

```text
Secure Hash Algorithm — 256 bit
```

It maps arbitrary-length input into a fixed-length digest:

$$
H=\operatorname{SHA256}(M).
$$

The resulting digest contains:

$$
256~\text{bits}=32~\text{bytes}.
$$

In hexadecimal representation:

$$
32\times2=64
$$

hexadecimal characters.

---

## 18. Hashing Architecture

```text
Message / Firmware
        ↓
      SHA-256
        ↓
256-bit Digest
```

A small input change should produce a significantly different digest.

Example:

```text
firmware_v1
     ↓
SHA-256
     ↓
Hash A

firmware_v2
     ↓
SHA-256
     ↓
Hash B
```

Usually:

$$
H_A\neq H_B.
$$

---

## 19. Experiment 7 — SHA-256 of a Message

Conceptually:

```cpp
const char* message =
    "ESP32 Secure System";
```

Calculate:

$$
H=
\operatorname{SHA256}
(
\text{``ESP32 Secure System''}
).
$$

Display the digest in hexadecimal format.

Example output structure:

```text
SHA-256:
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

The digest should contain 64 hexadecimal characters.

---

## 20. Experiment 8 — Demonstrate the Avalanche Effect

Calculate SHA-256 for:

```text
ESP32 Secure System
```

and then:

```text
ESP32 Secure system
```

Only one character has changed.

Compare the hashes.

Expected:

```text
Input changes slightly
        ↓
Hash changes significantly
```

This is known as the **avalanche effect**.

---

## 21. Firmware Integrity Verification

Suppose a trusted firmware file has the expected digest:

```text
Expected SHA-256
        ↓
A1B2C3...
```

The downloaded firmware is hashed:

```text
Downloaded Firmware
        ↓
SHA-256
        ↓
Calculated Hash
```

Verification:

$$
H_{\text{calculated}}
\stackrel{?}{=}
H_{\text{expected}}.
$$

---

## 22. Verification Decision

The verification logic is:

$$
V=
\begin{cases}
1, & H_{\text{calculated}}
=H_{\text{expected}},\\
0, & H_{\text{calculated}}
\neq H_{\text{expected}}.
\end{cases}
$$

Therefore:

```text
Hashes Match
     ↓
Firmware Integrity Valid

Hashes Differ
     ↓
Reject Firmware
```

---

## 23. Important Security Limitation of SHA-256

SHA-256 verifies that two byte sequences match a known digest, but a hash alone does **not** establish the identity of the firmware publisher.

For example, if an attacker can replace both:

```text
firmware.bin
```

and:

```text
firmware.sha256
```

then simple hash comparison may not provide authenticity.

A stronger production architecture is:

```text
Firmware
   ↓
SHA-256
   ↓
Digital Signature
   ↓
Verify Using Trusted Public Key
```

This concept is introduced later in this lab.

---

## 24. Experiment 9 — Firmware Hash Comparison

Assume:

```cpp
String expectedHash =
    "EXPECTED_64_CHARACTER_SHA256";
```

After downloading or reading the firmware, obtain:

```cpp
String calculatedHash =
    "...";
```

Then:

```cpp
if (
  calculatedHash.equalsIgnoreCase(
    expectedHash
  )
) {

  Serial.println(
    "Firmware integrity OK"
  );

}
else {

  Serial.println(
    "Firmware verification failed"
  );
}
```

---

## 25. Experiment 10 — Corruption Detection

Perform two tests.

### Test A — Correct Firmware

```text
Original Firmware
      ↓
SHA-256
      ↓
Match
      ↓
PASS
```

### Test B — Modified Firmware

Modify one or more bytes.

```text
Modified Firmware
      ↓
SHA-256
      ↓
Mismatch
      ↓
FAIL
```

Record:

| Firmware | Expected Hash | Calculated Hash | Result |
|---|---|---|---|
| Original |  |  | PASS |
| Modified |  |  | FAIL |

---

# 26. Part C — Over-the-Air Firmware Update

OTA stands for:

```text
Over-the-Air
```

OTA allows embedded devices to update firmware through a network without a physical USB connection.

Architecture:

```text
Firmware Server
       ↓
     Internet
       ↓
      Wi-Fi
       ↓
      ESP32
       ↓
 Firmware Download
       ↓
   Verification
       ↓
 Flash Installation
       ↓
      Restart
```

---

## 27. Basic OTA Workflow

A safer OTA sequence is:

```text
1. Connect to Wi-Fi
        ↓
2. Establish HTTPS Connection
        ↓
3. Retrieve Firmware Metadata
        ↓
4. Download Firmware
        ↓
5. Verify Firmware
        ↓
6. Write OTA Partition
        ↓
7. Finalize Update
        ↓
8. Restart
```

The critical security rule is:

```text
Verify Before Trusting New Firmware
```

---

## 28. Firmware Versioning

The existing firmware may contain:

```cpp
#define CURRENT_VERSION "1.0.0"
```

The server can advertise:

```text
1.1.0
```

Decision:

```text
Server Version > Current Version
            ↓
       Update Available
```

A production implementation should use a well-defined version-comparison policy rather than ordinary string comparison for arbitrary semantic versions.

---

## 29. Experiment 11 — Connect ESP32 to Wi-Fi

```cpp
#include <WiFi.h>

const char* ssid =
    "YOUR_WIFI";

const char* password =
    "YOUR_PASSWORD";

void connectWiFi() {

  WiFi.begin(
    ssid,
    password
  );

  while (
    WiFi.status()
    != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println(
    "Wi-Fi connected"
  );
}
```

For a teaching lab, credentials can be entered locally. Avoid committing real credentials to public repositories.

---

## 30. Experiment 12 — HTTPS Client

Use:

```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure client;
```

A secure deployment should validate the server certificate using a trusted CA certificate or another appropriate trust configuration.

Conceptually:

```text
ESP32
  ↓
TLS
  ↓
Server Certificate Verification
  ↓
HTTPS Server
```

Do not treat a configuration that disables certificate validation as a secure production OTA solution.

---

## 31. HTTPS vs. HTTP

| Feature | HTTP | HTTPS |
|---|---|---|
| Encryption | No | Yes |
| Server authentication | No | Yes, when certificates are correctly validated |
| Traffic confidentiality | No | Yes |
| Suitable for secure OTA | No | Preferred |

Firmware should not normally be distributed through unauthenticated plain HTTP.

---

## 32. Experiment 13 — Check Firmware Metadata

The server may provide metadata such as:

```json
{
  "version": "1.1.0",
  "firmware": "firmware.bin",
  "sha256": "..."
}
```

The ESP32 can process:

```text
Version
Firmware Location
Expected SHA-256
```

A stronger system would authenticate this metadata as well.

---

## 33. OTA Update Decision

```text
Read Current Version
        ↓
Read Server Version
        ↓
Newer Firmware?
   ├── No → Continue Running
   │
   └── Yes
          ↓
       Download
```

Conceptually:

```cpp
if (
  updateAvailable
) {

  performOTA();
}
```

---

## 34. Experiment 14 — Download Firmware

Conceptual ESP32 code:

```cpp
HTTPClient http;

http.begin(
    client,
    firmwareURL
);

int httpCode =
    http.GET();
```

Check:

```cpp
if (
  httpCode == HTTP_CODE_OK
) {

  Serial.println(
    "Firmware download started"
  );
}
```

The exact OTA streaming implementation depends on the ESP32 Arduino Core version and HTTP APIs available.

---

## 35. Firmware Size

Before writing the firmware, determine the binary size:

```cpp
int contentLength =
    http.getSize();
```

Verify that:

```text
Firmware Size
      ≤
Available OTA Partition
```

Otherwise, the update should be rejected.

---

## 36. ESP32 OTA Partitions

A typical OTA-capable flash layout may conceptually contain:

```text
┌─────────────────┐
│ Bootloader      │
├─────────────────┤
│ Partition Table │
├─────────────────┤
│ OTA Slot A      │
├─────────────────┤
│ OTA Slot B      │
├─────────────────┤
│ Data / NVS      │
└─────────────────┘
```

The running application occupies one slot while the new firmware can be written to another.

This helps avoid overwriting the currently executing image.

---

## 37. Experiment 15 — Begin Firmware Update

The Arduino `Update` component provides the firmware-writing mechanism.

```cpp
#include <Update.h>
```

Conceptually:

```cpp
if (
  !Update.begin(
    contentLength
  )
) {

  Serial.println(
    "Not enough space for OTA"
  );

  return;
}
```

---

## 38. Experiment 16 — Stream Firmware to Flash

The firmware data can be transferred from the HTTP stream into the update subsystem.

Conceptually:

```text
HTTPS Stream
     ↓
Firmware Bytes
     ↓
Update Writer
     ↓
OTA Partition
```

Depending on the ESP32 Arduino Core version, helper functions may support stream-based updates directly.

Always check returned byte counts and error status.

---

## 39. Experiment 17 — Firmware Verification

For the instructional architecture:

```text
Firmware Download
       ↓
SHA-256 Calculation
       ↓
Compare with Trusted Expected Hash
       ↓
Match?
  ├── No → Abort
  │
  └── Yes
         ↓
     Accept Firmware
```

The expected hash must itself come from a trusted source. Otherwise, hash comparison only detects accidental corruption rather than providing strong firmware authenticity.

---

## 40. Experiment 18 — Finalize the Update

Conceptually:

```cpp
if (
  Update.end()
) {

  Serial.println(
    "OTA update complete"
  );

}
else {

  Serial.println(
    "OTA update failed"
  );
}
```

Confirm that the update completed successfully before restart.

---

## 41. Restart After Successful Update

```cpp
ESP.restart();
```

The complete flow becomes:

```text
Download
    ↓
Verify
    ↓
Install
    ↓
Complete Successfully?
    ├── No → Stay on Existing Firmware
    │
    └── Yes
           ↓
         Restart
           ↓
       New Firmware
```

---

## 42. Experiment 19 — OTA Status LED

Use LEDs to indicate update status.

For example:

| State | LED Behavior |
|---|---|
| Normal | OFF |
| Downloading | Blink |
| Verification successful | ON |
| Verification failed | Fast blink |
| Update complete | ON for 2 s |

This makes the firmware-update process visible during the laboratory exercise.

---

## 43. OTA State Machine

The secure-update logic can be represented as a finite-state machine:

```text
IDLE
 ↓
CHECK_UPDATE
 ↓
CONNECT_SERVER
 ↓
DOWNLOAD
 ↓
VERIFY
 ├──── FAIL ────→ ERROR
 │
 └──── PASS
        ↓
      INSTALL
        ↓
      RESTART
```

Possible states:

```text
IDLE
CHECK
DOWNLOAD
VERIFY
INSTALL
SUCCESS
ERROR
```

---

## 44. Experiment 20 — Reject Invalid Firmware

Introduce a deliberately incorrect expected SHA-256 hash.

Expected behavior:

```text
Firmware Downloaded
       ↓
Calculated SHA-256
       ↓
Expected ≠ Calculated
       ↓
VERIFICATION FAILED
       ↓
OTA ABORTED
```

The current firmware should remain operational.

---

## 45. Security Layers

The complete system contains multiple layers:

```text
Application Security
      ↓
AES-256

Firmware Integrity
      ↓
SHA-256

Transport Security
      ↓
HTTPS / TLS

Firmware Authenticity
      ↓
Digital Signature

Platform Trust
      ↓
Secure Boot
```

Each layer solves a different problem.

---

## 46. AES vs. SHA-256

AES and SHA-256 are not interchangeable.

| Property | AES-256 | SHA-256 |
|---|---|---|
| Encryption | Yes | No |
| Decryption | Yes | No |
| Secret key | Yes | No |
| Fixed digest | No | Yes |
| Confidentiality | Yes | No |
| Integrity detection | Not by encryption alone | Yes, when compared against trusted hash |
| One-way function | No | Yes |

---

## 47. Secure Boot Concept

Secure Boot establishes trust before application firmware is executed.

Conceptually:

```text
Power On
   ↓
ROM / Root of Trust
   ↓
Verify Bootloader
   ↓
Verify Application
   ↓
Signature Valid?
   ├── No → Reject
   │
   └── Yes
          ↓
       Execute
```

Secure boot prevents unauthorized firmware from being executed when correctly provisioned.

---

## 48. Root of Trust

A **Root of Trust** is the trusted foundation from which other security decisions are derived.

Possible trusted elements include:

```text
ROM Code
Hardware Key Storage
eFuse
Trusted Public Key
Secure Bootloader
```

Architecture:

```text
Hardware Trust
      ↓
Bootloader Trust
      ↓
Firmware Trust
      ↓
Application Trust
```

---

## 49. Signed Firmware

A stronger secure-update architecture uses digital signatures.

```text
Developer
    ↓
Firmware
    ↓
SHA-256
    ↓
Sign with Private Key
    ↓
Firmware + Signature
```

On the device:

```text
Firmware + Signature
        ↓
Trusted Public Key
        ↓
Signature Verification
        ↓
Valid?
 ├── No → Reject
 │
 └── Yes
        ↓
     Install
```

The private signing key should remain protected and should not be placed on the device.

---

## 50. Hash Verification vs. Digital Signature

### SHA-256 Hash Comparison

```text
Firmware
   ↓
SHA-256
   ↓
Compare Hash
```

Provides:

```text
Integrity check
```

if the expected hash is trusted.

### Digital Signature

```text
Firmware
   ↓
Signature Verification
```

Provides stronger evidence of:

```text
Integrity
+
Authenticity
```

when the trusted public-key infrastructure is correctly managed.

---

## 51. Secure OTA Architecture

A stronger production design is:

```text
                    Firmware Developer
                           ↓
                     Build Firmware
                           ↓
                   Calculate SHA-256
                           ↓
                   Digitally Sign Image
                           ↓
                      HTTPS Server
                           ↓
                         ESP32
                           ↓
                   Validate TLS Server
                           ↓
                   Download Firmware
                           ↓
                 Verify Digital Signature
                           ↓
                    Verify Image
                           ↓
                     OTA Partition
                           ↓
                         Restart
                           ↓
                       Secure Boot
                           ↓
                       Application
```

---

## 52. Basic Secure OTA Used in This Lab

For instructional purposes, the laboratory uses:

```text
HTTPS
   +
SHA-256 Verification
   +
ESP32 OTA
```

represented as:

```text
Firmware Server
      ↓
HTTPS
      ↓
ESP32
      ↓
Download Firmware
      ↓
SHA-256 Verification
      ↓
Hash Match?
 ├── No → Abort
 │
 └── Yes
        ↓
     Install
        ↓
     Restart
```

This demonstrates the basic concepts, while signed firmware and secure boot provide stronger production-grade authenticity.

---

## 53. Experiment 21 — Complete Secure IoT Application

Combine the previous TinyML system with security:

```text
MPU6050
    ↓
TinyML Gesture Recognition
    ↓
Gesture Result
    ↓
AES-256 Encryption
    ↓
Network Communication
```

Firmware maintenance uses a separate path:

```text
Firmware Server
     ↓
HTTPS
     ↓
SHA-256 Verification
     ↓
OTA
     ↓
ESP32
```

The resulting system combines:

```text
Embedded Intelligence
        +
Connectivity
        +
Embedded Security
```

---

## 54. Experiments Summary

### Experiment 1 — AES Key

Define a 256-bit key.

### Experiment 2 — Plaintext

Prepare application data.

### Experiment 3 — AES Encryption

Encrypt the plaintext.

### Experiment 4 — Ciphertext

Display encrypted output.

### Experiment 5 — AES Decryption

Recover the original message.

### Experiment 6 — Sensor Encryption

Encrypt sensor data.

### Experiment 7 — SHA-256

Calculate a message digest.

### Experiment 8 — Avalanche Effect

Modify the message and compare hashes.

### Experiment 9 — Firmware Verification

Compare expected and calculated hashes.

### Experiment 10 — Corruption Detection

Verify that modified firmware is rejected.

### Experiment 11 — Wi-Fi

Connect the ESP32 to the network.

### Experiment 12 — HTTPS

Create a TLS-protected client.

### Experiment 13 — Firmware Metadata

Read version and hash information.

### Experiment 14 — Firmware Download

Retrieve the new firmware.

### Experiment 15 — OTA Initialization

Prepare the update partition.

### Experiment 16 — Firmware Write

Stream firmware into flash.

### Experiment 17 — Verification

Verify firmware before accepting the update.

### Experiment 18 — Update Completion

Finalize the OTA process.

### Experiment 19 — Status LED

Indicate firmware-update status.

### Experiment 20 — Invalid Firmware

Verify that an incorrect hash causes update rejection.

### Experiment 21 — Secure Intelligent Node

Combine TinyML, encrypted application data, and secure firmware maintenance.

---

## 55. Exercises

### Exercise 1 — AES-256 Message Encryption

Encrypt:

```text
ESP32 TinyML System
```

Display:

```text
Plaintext
Ciphertext
Recovered Plaintext
```

Verify that decryption reproduces the original data.

### Exercise 2 — Encrypt Sensor Data

Create:

```text
temperature=27.5
```

Then implement:

```text
Sensor Data
    ↓
AES-256
    ↓
Encrypted Payload
```

### Exercise 3 — SHA-256 Comparison

Calculate SHA-256 for:

```text
Firmware Version 1
```

and:

```text
Firmware Version 2
```

Compare the results.

### Exercise 4 — Firmware Integrity

Create:

```text
Firmware
   ↓
SHA-256
   ↓
Expected Hash
```

Modify the firmware and demonstrate that verification fails.

### Exercise 5 — Basic OTA

Implement:

```text
ESP32
   ↓
Wi-Fi
   ↓
HTTPS
   ↓
Firmware Server
   ↓
Download
   ↓
OTA
```

### Exercise 6 — Verified OTA

Extend the OTA pipeline:

```text
Download Firmware
       ↓
SHA-256 Check
       ↓
Valid?
 ├── No → Reject
 │
 └── Yes
        ↓
      Update
```

### Exercise 7 — Secure Embedded Intelligence

Construct:

```text
               MPU6050
                  ↓
               TinyML
                  ↓
           Gesture Decision
                  ↓
              AES-256
                  ↓
           Protected Data

                  +

           Firmware Server
                  ↓
                HTTPS
                  ↓
              SHA-256
                  ↓
                 OTA
                  ↓
                ESP32
```

---

## 56. Checkpoint Questions

1. What is embedded-system security?
2. What security property does AES primarily provide?
3. What does AES-256 mean?
4. How many bytes are required for an AES-256 key?
5. What is symmetric-key encryption?
6. What is an initialization vector?
7. Why should cryptographic keys be protected?
8. What is SHA-256?
9. How many bits are in a SHA-256 digest?
10. How many bytes are in a SHA-256 digest?
11. How many hexadecimal characters represent SHA-256?
12. What is the avalanche effect?
13. What is firmware integrity?
14. How can SHA-256 detect firmware modification?
15. Why does a plain hash alone not prove firmware authenticity?
16. What is OTA?
17. Why is OTA useful for embedded devices?
18. Why should HTTPS be used for firmware distribution?
19. Why is TLS certificate validation important?
20. What should happen when firmware hash verification fails?
21. What is an OTA partition?
22. Why should the existing firmware not immediately be overwritten?
23. What is Secure Boot?
24. What is a Root of Trust?
25. What is a digital signature?
26. What security advantage does signed firmware provide over a plain hash?
27. Why must the firmware signing private key remain secret?
28. What is the difference between confidentiality and integrity?
29. How do AES-256, SHA-256, HTTPS, and Secure Boot complement each other?
30. Why is embedded security important for an intelligent connected system?

---

## 57. Expected Results

Students should successfully encrypt application data:

```text
Plaintext
    ↓
AES-256
    ↓
Ciphertext
    ↓
AES-256
    ↓
Recovered Plaintext
```

Students should demonstrate firmware integrity verification:

```text
Firmware
    ↓
SHA-256
    ↓
Calculated Hash
    ↓
Compare
    ↓
PASS / FAIL
```

For valid firmware:

```text
Expected Hash
      =
Calculated Hash
      ↓
Firmware Accepted
```

For modified firmware:

```text
Expected Hash
      ≠
Calculated Hash
      ↓
Firmware Rejected
```

The OTA process should operate as:

```text
Firmware Server
      ↓
HTTPS
      ↓
ESP32
      ↓
Firmware Verification
      ↓
OTA Installation
      ↓
Restart
      ↓
New Firmware
```

---

## 58. Submission

Submit the following:

- AES-256 encryption source code.
- AES-256 decryption source code.
- Plaintext and ciphertext examples.
- Decryption verification.
- SHA-256 calculation code.
- SHA-256 digest results.
- Avalanche-effect experiment.
- Firmware SHA-256 verification results.
- Corrupted-firmware rejection demonstration.
- HTTPS connection implementation.
- OTA firmware code.
- Firmware version check.
- OTA verification logic.
- OTA status output.
- Screenshot of successful firmware update.
- Screenshot of rejected invalid firmware.
- Brief discussion of Secure Boot and Root of Trust.

Suggested AES result table:

| Test | Data | Encrypted | Decryption Correct |
|---:|---|---|---|
| 1 | Message | Yes |  |
| 2 | Temperature | Yes |  |
| 3 | Gesture result | Yes |  |

Suggested SHA-256 result table:

| Test | Input | Digest | Result |
|---:|---|---|---|
| 1 | Original |  |  |
| 2 | Modified |  |  |

Suggested OTA table:

| Test | Version | SHA-256 | Verification | OTA Result |
|---:|---|---|---|---|
| 1 | Valid | Match | PASS | Updated |
| 2 | Modified | Mismatch | FAIL | Rejected |
| 3 | Old version | Valid | — | No update |

---

## 59. Discussion

Students should discuss the distinction among the security mechanisms used in this laboratory.

AES-256 provides **confidentiality** by transforming readable application data into ciphertext using a secret key. SHA-256 provides a cryptographic digest that can be used to detect modification when compared with a trusted expected value. HTTPS protects firmware while it is transported between the server and the device, assuming TLS certificates are properly validated.

However, firmware integrity and firmware authenticity are not identical. A SHA-256 hash comparison is useful only when the expected hash itself is obtained through a trusted mechanism. Production systems therefore commonly combine authenticated metadata or **digitally signed firmware** with secure boot and protected key storage.

Students should also consider the engineering trade-offs among:

$$
\boxed{
\text{Security}
\leftrightarrow
\text{Memory}
\leftrightarrow
\text{Computation}
\leftrightarrow
\text{Energy}
}
$$

which are particularly important for resource-constrained embedded systems.

---

## 60. Conclusion

In this lab, the ESP32 is extended from an intelligent embedded device into a **security-aware connected embedded system**. Application data are protected using AES-256 encryption, while SHA-256 hashing is used to detect firmware modification. HTTPS provides transport protection for firmware delivery, and the ESP32 OTA mechanism enables firmware to be updated remotely.

The lab also establishes an important distinction between **integrity and authenticity**. SHA-256 can detect modification when compared against a trusted digest, but stronger production systems should authenticate firmware through digital signatures and establish a Root of Trust using mechanisms such as secure boot and protected key storage.

The complete course progression becomes:

```text
LAB 13
IMU Gesture Data Collection
      ↓
LAB 14
TinyML Training & Optimization
      ↓
LAB 15
Real-Time Edge Inference
      ↓
LAB 16
AES-256 Data Protection
      ↓
SHA-256 Firmware Verification
      ↓
Secure OTA Firmware Update
      ↓
Secure Embedded Intelligence
```

This laboratory completes the core pipeline:

$$
\boxed{
\text{Sense}
\rightarrow
\text{Connect}
\rightarrow
\text{Learn}
\rightarrow
\text{Infer}
\rightarrow
\text{Act}
\rightarrow
\text{Protect}
}
$$

by integrating **embedded intelligence, connected communication, cryptographic protection, firmware integrity verification, and secure device maintenance** into a single MCU-based system.
