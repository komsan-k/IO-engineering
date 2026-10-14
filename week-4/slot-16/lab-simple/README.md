# LAB — Simple Embedded Security on ESP32
## AES-256, SHA-256, and Basic OTA Update

## 1. Objective

The objective of this lab is to:

1. Understand basic embedded-system security.
2. Encrypt and decrypt a short message using **AES-256**.
3. Compute a **SHA-256** hash for a message.
4. Detect whether data have been modified.
5. Understand the basic idea of **Over-the-Air (OTA)** firmware update.
6. Connect the ESP32 to Wi-Fi and prepare it for a simple firmware update workflow.

---

## 2. Learning Outcomes

After completing this lab, students should be able to:

- explain confidentiality and integrity;
- explain the role of AES-256;
- explain the role of SHA-256;
- encrypt and decrypt simple application data;
- calculate and compare SHA-256 hashes;
- describe the basic OTA firmware-update process.

---

## 3. Basic Security Concepts

This lab focuses on three ideas:

```text
AES-256
   ↓
Confidentiality

SHA-256
   ↓
Integrity

OTA
   ↓
Remote Firmware Update
```

### Security Properties

| Property | Meaning | Example |
|---|---|---|
| Confidentiality | Prevent unauthorized reading | AES-256 |
| Integrity | Detect modification | SHA-256 |
| Firmware maintenance | Update software remotely | OTA |

---

## 4. Hardware Requirements

- ESP32 development board
- USB cable
- Computer
- Wi-Fi network

Optional:

- LED for OTA status indication

---

## 5. Software Requirements

- Arduino IDE
- ESP32 Arduino Core
- Serial Monitor

Useful libraries/components:

```cpp
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
```

For cryptographic functions, the ESP32 commonly provides **mbedTLS**.

---

# Part A — AES-256 Encryption

## 6. What is AES-256?

AES stands for:

```text
Advanced Encryption Standard
```

AES is a symmetric-key encryption algorithm.

```text
Plaintext
   ↓
AES-256 Encryption
   ↓
Ciphertext
   ↓
AES-256 Decryption
   ↓
Plaintext
```

AES-256 uses a:

```text
256-bit key
=
32-byte key
```

---

## 7. Experiment 1 — Define a Key

For this laboratory demonstration, use a fixed 32-byte key.

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

> This key is intentionally visible for teaching. Do not hard-code production secret keys in real systems.

---

## 8. Experiment 2 — Prepare Plaintext

Use:

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
Encrypted Data
```

The encrypted output should not look like the original message.

---

## 9. Experiment 3 — Encrypt and Decrypt

Conceptual flow:

```text
Initialize AES
     ↓
Load 256-bit Key
     ↓
Encrypt Plaintext
     ↓
Ciphertext
     ↓
Decrypt Ciphertext
     ↓
Recovered Plaintext
```

Expected result:

```text
Original:
temperature=27.5

Recovered:
temperature=27.5
```

If the recovered text is identical to the original text, the encryption/decryption process is working.

---

## 10. Display Ciphertext as Hex

Binary ciphertext should be displayed in hexadecimal.

```cpp
void printHex(
    const unsigned char* data,
    size_t length
) {
  for (size_t i = 0; i < length; i++) {

    if (data[i] < 16) {
      Serial.print("0");
    }

    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}
```

Example output:

```text
Plaintext:
temperature=27.5

Ciphertext:
A4 73 19 D2 8F ...
```

---

## 11. AES Result Table

| Test | Plaintext | Encrypted | Decryption Correct |
|---:|---|---|---|
| 1 | Hello ESP32 | Yes | |
| 2 | temperature=27.5 | Yes | |
| 3 | humidity=68 | Yes | |

---

# Part B — SHA-256 Integrity Check

## 12. What is SHA-256?

SHA-256 stands for:

```text
Secure Hash Algorithm — 256 bit
```

It converts input data into a fixed-size digest:

```text
Message
   ↓
SHA-256
   ↓
256-bit Digest
```

A SHA-256 digest contains:

```text
256 bits
=
32 bytes
=
64 hexadecimal characters
```

---

## 13. Experiment 4 — Calculate SHA-256

Use the message:

```text
ESP32 Secure System
```

Conceptual code flow:

```text
Message
   ↓
SHA-256
   ↓
Hash
```

Display the digest in hexadecimal.

Example format:

```text
SHA-256:
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
```

---

## 14. Experiment 5 — Detect Data Modification

Calculate SHA-256 for:

```text
ESP32 Secure System
```

Then change one character:

```text
ESP32 Secure system
```

Compare the results.

Expected:

```text
Small Input Change
       ↓
Very Different Hash
```

This demonstrates the **avalanche effect**.

---

## 15. Simple Integrity Verification

Suppose we have:

```text
Expected Hash
```

and:

```text
Calculated Hash
```

Decision:

```text
Hashes Match
    ↓
PASS

Hashes Different
    ↓
FAIL
```

Conceptual code:

```cpp
if (
  calculatedHash.equalsIgnoreCase(
    expectedHash
  )
) {
  Serial.println(
    "Integrity OK"
  );
}
else {
  Serial.println(
    "Integrity FAILED"
  );
}
```

---

## 16. SHA-256 Result Table

| Test | Input | Result |
|---:|---|---|
| 1 | Original message | |
| 2 | Modified message | |

---

# Part C — Basic OTA Concept

## 17. What is OTA?

OTA stands for:

```text
Over-the-Air
```

It allows firmware to be updated through a network.

```text
Firmware Server
      ↓
Wi-Fi
      ↓
ESP32
      ↓
Download Firmware
      ↓
Install
      ↓
Restart
```

---

## 18. Basic OTA Workflow

A simple OTA workflow is:

```text
1. Connect to Wi-Fi
        ↓
2. Connect to Firmware Server
        ↓
3. Download Firmware
        ↓
4. Verify Firmware
        ↓
5. Install Firmware
        ↓
6. Restart ESP32
```

The important rule is:

```text
Verify Firmware Before Installation
```

---

## 19. Experiment 6 — Connect ESP32 to Wi-Fi

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

Expected output:

```text
....
Wi-Fi connected
```

---

## 20. HTTPS for Firmware Download

Use:

```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure client;
```

A secure system should validate the firmware server certificate.

Conceptually:

```text
ESP32
  ↓
HTTPS / TLS
  ↓
Firmware Server
```

---

## 21. Simple Firmware Verification

Before installation:

```text
Downloaded Firmware
      ↓
SHA-256
      ↓
Compare with Expected Hash
      ↓
Match?
 ├── No → Reject
 └── Yes → Install
```

This protects against accidental or unauthorized modification only when the expected hash itself comes from a trusted source.

---

## 22. Basic OTA Decision

```text
New Firmware Available?
      ↓
     Yes
      ↓
Download
      ↓
Verify
      ↓
Install
      ↓
Restart
```

If verification fails:

```text
Verification Failed
      ↓
Abort Update
```

The current firmware should remain operational.

---

# 23. Complete Simple Security Architecture

```text
Application Data
      ↓
AES-256
      ↓
Encrypted Data


Firmware / Message
      ↓
SHA-256
      ↓
Integrity Check


Firmware Server
      ↓
HTTPS
      ↓
ESP32
      ↓
Verify
      ↓
OTA Update
```

---

## 24. Experiment Summary

### Experiment 1

Define a 256-bit AES key.

### Experiment 2

Prepare plaintext.

### Experiment 3

Encrypt and decrypt the message.

### Experiment 4

Calculate SHA-256.

### Experiment 5

Modify the message and compare hashes.

### Experiment 6

Connect ESP32 to Wi-Fi and explain the OTA workflow.

---

## 25. Checkpoint Questions

1. What security property does AES-256 provide?
2. How many bytes are in an AES-256 key?
3. What is plaintext?
4. What is ciphertext?
5. What is SHA-256?
6. How many bits are in a SHA-256 digest?
7. What is the avalanche effect?
8. What is the difference between encryption and hashing?
9. What does OTA mean?
10. Why should firmware be verified before installation?
11. Why is HTTPS preferred over HTTP for firmware download?
12. What should happen if firmware verification fails?

---

## 26. Assignment

Create a simple ESP32 security demonstration that includes:

```text
Message
  ↓
AES-256
  ↓
Encrypted Message

and

Message
  ↓
SHA-256
  ↓
Hash
```

Then explain the basic OTA workflow:

```text
Firmware Server
      ↓
HTTPS
      ↓
ESP32
      ↓
Verify
      ↓
Update
```

---

## 27. Submission

Submit:

- AES-256 encryption code
- AES-256 decryption result
- plaintext and ciphertext output
- SHA-256 calculation result
- original vs modified hash comparison
- Wi-Fi connection code
- simple OTA workflow diagram
- short discussion

---

## 28. Expected Results

Students should demonstrate:

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

and:

```text
Message
   ↓
SHA-256
   ↓
Digest
```

A modified message should produce a different digest.

Students should also be able to explain:

```text
Firmware Server
      ↓
HTTPS
      ↓
ESP32
      ↓
Verification
      ↓
OTA Update
```

---

## 29. Conclusion

This simplified laboratory introduces three basic embedded-security mechanisms:

```text
AES-256
   ↓
Confidentiality

SHA-256
   ↓
Integrity

OTA
   ↓
Remote Firmware Maintenance
```

The lab intentionally focuses on the basic concepts rather than a production-grade secure-update system. More advanced topics such as digital signatures, Secure Boot, protected key storage, and Root of Trust can be introduced in a later laboratory.
