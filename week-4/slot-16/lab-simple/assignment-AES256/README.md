# LAB — Encrypted IoT Sensor Communication using ESP32, AES-256, and MQTT

## 1. Objective

The objective of this laboratory is to extend basic ESP32 sensor communication by protecting LDR data before transmission through MQTT.

Students will:

1. Read ambient-light data from an LDR using the ESP32 ADC.
2. Convert the sensor reading into an application message.
3. Encrypt the message using **AES-256-CBC**.
4. Encode the binary ciphertext as hexadecimal text.
5. Publish the encrypted payload through MQTT.
6. Receive the encrypted payload using another ESP32.
7. Decode and decrypt the payload.
8. Recover the original LDR value.
9. Compare plain MQTT with application-level encrypted MQTT.
10. Understand the roles of encryption, MQTT authentication, and TLS.

---

## 2. Learning Outcomes

After completing this laboratory, students should be able to:

- explain application-level IoT data encryption;
- describe the MQTT publish/subscribe architecture;
- distinguish plaintext and ciphertext;
- explain AES-256 keys and CBC initialization vectors;
- encrypt sensor data before MQTT transmission;
- encode binary ciphertext into hexadecimal text;
- decrypt an MQTT payload on a receiving ESP32;
- compare payload encryption with MQTT over TLS;
- explain why encryption alone does not guarantee message authenticity.

---

## 3. System Architecture

The basic system uses two ESP32 boards.

```text
                     MQTT Broker
                  ┌───────────────┐
                  │               │
                  └───────┬───────┘
                          │
                    MQTT Network
                 ┌────────┴────────┐
                 │                 │
                 │                 │
             Publish           Subscribe
                 │                 │
                 │                 │
        ┌────────▼──────┐  ┌──────▼─────────┐
        │    ESP32 A    │  │    ESP32 B     │
        │   Publisher   │  │   Subscriber   │
        └───────┬───────┘  └──────┬─────────┘
                │                  │
             AES-256            AES-256
             Encrypt            Decrypt
                │                  │
               LDR            Recovered LDR
```

---

## 4. Secure Sensor Communication Flow

```text
Ambient Light
      ↓
     LDR
      ↓
ESP32 ADC
      ↓
Application Message
      ↓
"ldr=742"
      ↓
AES-256-CBC
      ↓
Binary Ciphertext
      ↓
HEX Encoding
      ↓
MQTT Publish
      ↓
MQTT Broker
      ↓
MQTT Subscribe
      ↓
HEX Decoding
      ↓
AES-256-CBC
      ↓
"ldr=742"
```

---

## 5. Security Layers

Three different mechanisms should be distinguished.

| Security Layer | Mechanism | Purpose |
|---|---|---|
| Application | AES-256 | Protect sensor payload |
| MQTT | Username/password | Broker access control |
| Transport | MQTT over TLS | Protect network connection |

---

## 6. Hardware Requirements

For the complete experiment:

- 2 × ESP32 development boards
- 1 × LDR
- 1 × 10 kΩ resistor
- Breadboard
- Jumper wires
- USB cables
- Wi-Fi network
- MQTT broker

A single ESP32 can also be used initially to test publishing.

---

## 7. Software Requirements

- Arduino IDE
- ESP32 Arduino Core
- MQTT library such as `PubSubClient`
- mbedTLS
- Serial Monitor
- MQTT broker

Libraries:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include "mbedtls/aes.h"
```

---

## 8. LDR Circuit

A simple voltage divider can be used.

```text
3.3 V
  |
 LDR
  |
  +-------- ADC GPIO
  |
10 kΩ
  |
 GND
```

Example:

```cpp
#define LDR_PIN 36
```

For ESP32-S3, select an ADC-capable GPIO appropriate to the specific development board.

---

## 9. MQTT Configuration

Use a simple topic:

```text
iot/lab/ldr/encrypted
```

Optional plaintext topic for comparison:

```text
iot/lab/ldr/plain
```

Architecture:

```text
ESP32 Publisher
      ↓
iot/lab/ldr/encrypted
      ↓
MQTT Broker
      ↓
ESP32 Subscriber
```

---

## 10. Experiment 1 — Read LDR Data

Basic Arduino code:

```cpp
int ldrValue =
    analogRead(
        LDR_PIN
    );

Serial.print(
    "LDR = "
);

Serial.println(
    ldrValue
);
```

Example:

```text
LDR = 742
```

---

## 11. Experiment 2 — Create Sensor Message

Convert the reading into an application message.

```cpp
String message =
    "ldr="
    +
    String(
        ldrValue
    );
```

Example:

```text
ldr=742
```

A more advanced message may be:

```text
device=esp32_01,ldr=742
```

---

## 12. AES-256 Configuration

AES-256 requires a 256-bit key:

\[
256\text{ bits}=32\text{ bytes}
\]

Example laboratory key:

```cpp
const unsigned char aesKey[32] =
{
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

---

## 13. Initialization Vector

AES-CBC requires a 16-byte IV.

```cpp
const unsigned char initialIV[16] =
{
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};
```

For the teaching experiment, a fixed IV simplifies implementation.

For a real system, the IV should not simply be reused for every message when the selected mode requires uniqueness or unpredictability.

---

## 14. Why Encode Ciphertext?

AES produces binary output.

For example:

```text
10100110 00110110 ...
```

Binary data may contain zero bytes or control characters that are inconvenient to transmit as ordinary text.

Therefore:

```text
Binary Ciphertext
       ↓
HEX Encoding
       ↓
"4A91F230..."
```

The encoded text can then be transmitted easily through MQTT.

---

## 15. Experiment 3 — Binary-to-Hex Conversion

```cpp
String bytesToHex(
    const unsigned char* data,
    size_t length
)
{
    String result = "";

    char buffer[3];

    for (
        size_t i = 0;
        i < length;
        i++
    )
    {
        sprintf(
            buffer,
            "%02X",
            data[i]
        );

        result += buffer;
    }

    return result;
}
```

Example:

```text
Ciphertext:

A4 73 19 D2

HEX MQTT payload:

A47319D2
```

---

## 16. Experiment 4 — Hex-to-Binary Conversion

The subscriber performs the reverse operation.

```cpp
int hexValue(
    char c
)
{
    if (
        c >= '0'
        &&
        c <= '9'
    )
        return c - '0';

    if (
        c >= 'A'
        &&
        c <= 'F'
    )
        return c - 'A' + 10;

    if (
        c >= 'a'
        &&
        c <= 'f'
    )
        return c - 'a' + 10;

    return -1;
}
```

Conceptually:

```text
"A47319D2"
      ↓
A4 73 19 D2
      ↓
AES Decryption
```

---

## 17. Publisher Processing Pipeline

```text
analogRead()
    ↓
LDR Value
    ↓
String("ldr=742")
    ↓
PKCS#7 Padding
    ↓
AES-256-CBC Encryption
    ↓
HEX Encoding
    ↓
mqtt.publish()
```

---

## 18. Subscriber Processing Pipeline

```text
MQTT Callback
     ↓
Encrypted HEX Payload
     ↓
HEX Decoding
     ↓
Ciphertext Bytes
     ↓
AES-256-CBC Decryption
     ↓
Remove PKCS#7 Padding
     ↓
Recovered Message
     ↓
ldr=742
```

---

## 19. Complete Publisher Architecture

```text
                ESP32 Publisher

                   LDR
                    ↓
                ADC Reading
                    ↓
             Create Message
                    ↓
              AES-256-CBC
                    ↓
              HEX Encoding
                    ↓
                  MQTT
                    ↓
           iot/lab/ldr/encrypted
```

---

## 20. Publisher Code Structure

The main loop can follow:

```cpp
void loop()
{
    if (
        !mqttClient.connected()
    )
    {
        reconnectMQTT();
    }

    mqttClient.loop();

    int ldrValue =
        analogRead(
            LDR_PIN
        );

    String message =
        "ldr="
        +
        String(
            ldrValue
        );

    String encrypted =
        encryptToHex(
            message
        );

    mqttClient.publish(
        "iot/lab/ldr/encrypted",
        encrypted.c_str()
    );

    delay(
        2000
    );
}
```

---

## 21. MQTT Connection

```cpp
WiFiClient wifiClient;

PubSubClient mqttClient(
    wifiClient
);
```

Setup:

```cpp
mqttClient.setServer(
    mqttServer,
    1883
);
```

Example:

```cpp
const char* mqttServer =
    "192.168.1.100";
```

---

## 22. Connect to Wi-Fi

```cpp
void connectWiFi()
{
    WiFi.begin(
        ssid,
        password
    );

    while (
        WiFi.status()
        !=
        WL_CONNECTED
    )
    {
        delay(
            500
        );

        Serial.print(
            "."
        );
    }

    Serial.println();

    Serial.println(
        "Wi-Fi connected"
    );
}
```

---

## 23. Connect to MQTT Broker

```cpp
void reconnectMQTT()
{
    while (
        !mqttClient.connected()
    )
    {
        Serial.println(
            "Connecting to MQTT..."
        );

        if (
            mqttClient.connect(
                "ESP32_LDR_Publisher"
            )
        )
        {
            Serial.println(
                "MQTT connected"
            );
        }
        else
        {
            delay(
                2000
            );
        }
    }
}
```

---

## 24. Experiment 5 — Plain MQTT

First publish without encryption.

```cpp
mqttClient.publish(
    "iot/lab/ldr/plain",
    message.c_str()
);
```

MQTT subscriber sees:

```text
ldr=742
```

Observation:

```text
Anyone able to inspect the MQTT payload
can directly read the sensor value.
```

---

## 25. Experiment 6 — Encrypted MQTT

Now publish:

```cpp
mqttClient.publish(
    "iot/lab/ldr/encrypted",
    encryptedPayload.c_str()
);
```

Subscriber sees:

```text
8A71DCD93E23...
```

The original LDR value is no longer directly visible.

---

## 26. Experiment 7 — MQTT Subscriber

The subscriber registers:

```cpp
mqttClient.setCallback(
    callback
);
```

and subscribes:

```cpp
mqttClient.subscribe(
    "iot/lab/ldr/encrypted"
);
```

---

## 27. Subscriber Callback

Conceptually:

```cpp
void callback(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    String encrypted = "";

    for (
        unsigned int i = 0;
        i < length;
        i++
    )
    {
        encrypted +=
            (char) payload[i];
    }

    String recovered =
        decryptFromHex(
            encrypted
        );

    Serial.print(
        "Recovered: "
    );

    Serial.println(
        recovered
    );
}
```

---

## 28. Expected Publisher Output

```text
Wi-Fi connected
MQTT connected

LDR:
742

Plaintext:
ldr=742

Encrypted:
5A7D92C84F5E...

Published:
iot/lab/ldr/encrypted
```

---

## 29. Expected Subscriber Output

```text
MQTT message received

Encrypted:
5A7D92C84F5E...

Decrypted:
ldr=742
```

---

## 30. Experiment 8 — Compare Plain and Encrypted MQTT

Publish the same data to both topics:

```text
iot/lab/ldr/plain

iot/lab/ldr/encrypted
```

Example:

```text
Plain Topic:

ldr=742


Encrypted Topic:

4F7C2A9E...
```

Record:

| Method | MQTT Payload | Human Readable |
|---|---|---|
| Plain MQTT | `ldr=742` | Yes |
| AES MQTT | `7A83F2...` | No |

---

## 31. Experiment 9 — Wrong AES Key

Change one byte in the subscriber key.

For example:

```cpp
0x10
```

to:

```cpp
0x11
```

Expected:

```text
Publisher Key
    ≠
Subscriber Key

        ↓

Incorrect Plaintext
or
Padding Failure
```

This demonstrates that both endpoints need the same symmetric key.

---

## 32. Experiment 10 — Modified Ciphertext

Change one hexadecimal character in the MQTT payload.

```text
Original:

5A7D92...

Modified:

5A7E92...
```

AES-CBC decryption may produce corrupted plaintext.

This experiment demonstrates an important limitation:

```text
Encryption
≠
Authentication
```

---

## 33. Why AES-CBC Alone is Not Enough

AES-CBC provides confidentiality, but does not by itself prove:

```text
Who sent the message?
```

or guarantee:

```text
The ciphertext was not intentionally modified.
```

A stronger design can use:

```text
AES-CBC
   +
HMAC
```

or an authenticated encryption mode such as:

```text
AES-GCM
```

---

## 34. Extension — MQTT over TLS

The current architecture is:

```text
LDR
 ↓
AES-256
 ↓
MQTT
 ↓
Broker
```

It can be extended to:

```text
LDR
 ↓
AES-256
 ↓
MQTT
 ↓
TLS
 ↓
Broker
```

Result:

```text
Application Payload Security
          +
Transport Security
```

---

## 35. Security Comparison

| Configuration | Payload Encryption | Transport Encryption |
|---|---|---|
| Plain MQTT | No | No |
| AES + MQTT | Yes | No |
| MQTT/TLS | No application encryption | Yes |
| AES + MQTT/TLS | Yes | Yes |

---

## 36. Extension — JSON Before Encryption

Instead of:

```text
ldr=742
```

use:

```json
{
  "device": "esp32_01",
  "ldr": 742
}
```

Then:

```text
JSON
 ↓
AES
 ↓
MQTT
```

The broker only receives ciphertext.

---

## 37. Extension — Multiple Sensors

The same architecture can protect:

```text
LDR
Temperature
Humidity
Motion
IMU
```

Example plaintext:

```text
ldr=742,temp=27.5,humidity=68
```

or:

```json
{
  "ldr": 742,
  "temperature": 27.5,
  "humidity": 68
}
```

---

## 38. Extension — TinyML Classification

The system can also integrate the previous LDR ANN laboratory.

```text
LDR
 ↓
ANN
 ↓
Dark / Normal / Bright
 ↓
AES-256
 ↓
MQTT
```

Example:

```text
prediction=normal
```

becomes encrypted before transmission.

A more complete intelligent IoT node is:

```text
              LDR
               ↓
         Feature Extraction
               ↓
              ANN
               ↓
      Dark / Normal / Bright
               ↓
            AES-256
               ↓
             MQTT
               ↓
             Broker
```

---

## 39. Experiment 11 — Measure Encryption Time

Use:

```cpp
unsigned long startTime =
    micros();

encryptMessage();

unsigned long elapsed =
    micros()
    -
    startTime;
```

Record:

| Test | Encryption Time (µs) |
|---:|---:|
| 1 | |
| 2 | |
| 3 | |
| 4 | |
| 5 | |
| Mean | |

---

## 40. Experiment 12 — Message Size Overhead

HEX encoding approximately doubles the binary ciphertext representation because one byte is represented by two hexadecimal characters.

Record:

| Data | Size |
|---|---:|
| Plain message | |
| Padded plaintext | |
| Ciphertext | |
| HEX MQTT payload | |

Discuss the trade-off:

\[
\text{Security}
\leftrightarrow
\text{Communication Overhead}
\]

---

## 41. Experiments Summary

### Experiment 1
Read LDR.

### Experiment 2
Create sensor message.

### Experiment 3
Encrypt using AES-256.

### Experiment 4
Encode ciphertext using HEX.

### Experiment 5
Publish plaintext MQTT.

### Experiment 6
Publish encrypted MQTT.

### Experiment 7
Receive and decrypt MQTT.

### Experiment 8
Compare plain and encrypted MQTT.

### Experiment 9
Test incorrect AES key.

### Experiment 10
Modify ciphertext.

### Experiment 11
Measure encryption time.

### Experiment 12
Measure communication overhead.

---

## 42. Checkpoint Questions

1. What is MQTT?
2. What is the role of the MQTT broker?
3. What is a topic?
4. What does publish mean?
5. What does subscribe mean?
6. What security property does AES provide?
7. Why does AES-256 require a 32-byte key?
8. Why is an IV required for CBC mode?
9. Why is PKCS#7 padding necessary?
10. Why is ciphertext converted to HEX?
11. Why must the publisher and subscriber use the same AES key?
12. Why should the ESP32 not publish the secret key?
13. What is the difference between plain MQTT and encrypted MQTT?
14. Does AES-CBC alone provide authentication?
15. What is MQTT over TLS?
16. What is the difference between payload encryption and transport encryption?
17. Why might AES-GCM be preferable to AES-CBC?
18. What happens when ciphertext is modified?
19. What is the communication overhead of HEX encoding?
20. How could TinyML classification be integrated with this system?

---

## 43. Assignment

Develop a secure LDR MQTT system:

```text
LDR
 ↓
ESP32 Publisher
 ↓
AES-256-CBC
 ↓
HEX
 ↓
MQTT
 ↓
Broker
 ↓
ESP32 Subscriber
 ↓
HEX Decode
 ↓
AES-256-CBC
 ↓
Recovered LDR
```

Students should demonstrate at least:

```text
10 different LDR readings
```

---

## 44. Submission

Submit:

- publisher Arduino code;
- subscriber Arduino code;
- AES-256 key configuration;
- LDR circuit;
- MQTT topic configuration;
- plaintext MQTT result;
- encrypted MQTT result;
- decrypted subscriber result;
- screenshot of MQTT communication;
- wrong-key experiment;
- modified-ciphertext experiment;
- encryption-time measurements;
- message-size comparison;
- brief security discussion.

---

## 45. Expected Results

For an LDR value such as:

```text
742
```

the publisher should create:

```text
Plaintext:
ldr=742
```

Then:

```text
AES-256
 ↓
Ciphertext
```

and publish something similar to:

```text
5A7832F4D9...
```

The subscriber should recover:

```text
ldr=742
```

Thus:

```text
Original
   =
Recovered
```

while the MQTT network carries only the encrypted payload.

---

## 46. Discussion

This laboratory demonstrates that MQTT provides a messaging mechanism, while AES provides application-level confidentiality.

Without encryption:

```text
ESP32
 ↓
MQTT
 ↓
ldr=742
```

the application data are directly visible.

With encryption:

```text
ESP32
 ↓
AES-256
 ↓
MQTT
 ↓
A94E3C...
```

the sensor data are protected even at the payload level.

However, an application-level encryption scheme should not be considered a complete IoT security architecture. Key management, device authentication, replay protection, message integrity, TLS certificate validation, and secure firmware should also be considered.

---

## 47. Conclusion

This laboratory extends basic sensor networking into **secure IoT communication** by combining:

\[
\boxed{
\text{Sensing}
\rightarrow
\text{Encryption}
\rightarrow
\text{Communication}
\rightarrow
\text{Decryption}
}
\]

The complete workflow is:

```text
Ambient Light
      ↓
     LDR
      ↓
    ESP32
      ↓
Application Message
      ↓
AES-256-CBC
      ↓
HEX Encoding
      ↓
MQTT Publish
      ↓
MQTT Broker
      ↓
MQTT Subscribe
      ↓
HEX Decode
      ↓
AES-256-CBC
      ↓
Recovered Sensor Data
```

This laboratory also provides a useful progression toward more advanced secure IoT systems:

```text
Plain Sensor
     ↓
Plain MQTT
     ↓
AES-Encrypted MQTT
     ↓
MQTT over TLS
     ↓
AES-GCM / Authenticated Encryption
     ↓
Secure Intelligent IoT Node
```
