# 📘 Theory: Hardware Security, Cryptography, Secure Boot, and Root of Trust  
## ESP32 MCU Case Study

---

## 🧩 1. Introduction

Modern microcontrollers are increasingly connected to networks, cloud services, sensors, actuators, and other embedded devices. This connectivity improves functionality but also increases the **attack surface** of embedded systems.

An attacker may attempt to:

- read confidential data,
- modify firmware,
- impersonate a legitimate device,
- extract cryptographic keys,
- install malicious firmware,
- intercept network communication,
- bypass authentication,
- manipulate sensor or actuator behavior.

For an ESP32-based embedded system, security should therefore be considered from the hardware level through firmware and communication protocols.

A simplified security architecture is:

```text
                Embedded Security
                       │
        ┌──────────────┼──────────────┐
        ▼              ▼              ▼
 Hardware Security  Software       Communication
        │            Security         Security
        ▼              ▼              ▼
 Secure Boot      Authentication    Encryption
 Flash Encryption Access Control    TLS
 Crypto Hardware  Firmware Check    Secure MQTT
 Root of Trust    Updates           HTTPS
```

A secure embedded system can be represented conceptually as:

$$
\boxed{
\text{Embedded Security} =
\text{Hardware Trust}
+
\text{Cryptography}
+
\text{Secure Firmware}
+
\text{Secure Communication}
}
$$

---

# 🔐 2. Security Goals

Embedded-system security is commonly based on three fundamental properties:

$$
\boxed{
\text{CIA} =
\text{Confidentiality}
+
\text{Integrity}
+
\text{Availability}
}
$$

### Confidentiality

Confidentiality prevents unauthorized users from accessing information.

Examples include:

- passwords,
- cryptographic keys,
- sensor data,
- personal information,
- network credentials.

Encryption is commonly used to protect confidentiality.

---

### Integrity

Integrity ensures that information has not been modified without authorization.

For example:

```text
Original Firmware
       │
       ▼
Integrity Verification
       │
       ├── Valid ─────► Execute
       │
       └── Invalid ───► Reject
```

Hashes, message authentication codes, and digital signatures can contribute to integrity protection.

---

### Availability

Availability ensures that a system remains operational when legitimate users require it.

Threats include:

- denial-of-service attacks,
- resource exhaustion,
- communication flooding,
- malicious firmware,
- power attacks.

---

# 🛡️ 3. Additional Security Properties

Embedded security also involves:

### Authentication

Verifying the identity of a device or user.

$$
\boxed{
\text{Authentication}
\rightarrow
\text{Who are you?}
}
$$

### Authorization

Determining what an authenticated entity is allowed to do.

$$
\boxed{
\text{Authorization}
\rightarrow
\text{What are you allowed to do?}
}
$$

### Authenticity

Ensuring that data or firmware originates from a trusted source.

### Non-repudiation

Providing evidence that an entity performed a particular action, typically through appropriate digital-signature mechanisms.

---

# ⚙️ 4. Hardware Security

**Hardware security** uses hardware-supported mechanisms to protect the MCU, firmware, keys, and data.

A simplified architecture is:

```text
┌─────────────────────────────────────┐
│              ESP32 MCU              │
│                                     │
│   ┌───────────┐    ┌────────────┐   │
│   │    CPU    │    │ Crypto     │   │
│   │           │    │ Hardware   │   │
│   └───────────┘    └────────────┘   │
│                                     │
│   ┌───────────┐    ┌────────────┐   │
│   │ Security  │    │ Security   │   │
│   │ Settings  │    │ Material   │   │
│   └───────────┘    └────────────┘   │
│                                     │
│   ┌─────────────────────────────┐   │
│   │ Secure Boot / Flash         │   │
│   │ Protection Mechanisms       │   │
│   └─────────────────────────────┘   │
└─────────────────────────────────────┘
```

Specific security features vary among members of the ESP32 family, so designers should consult the documentation for the exact target chip.

---

# 🧱 5. Hardware vs. Software Security

Security mechanisms may be implemented primarily in software or supported directly by hardware.

| Feature | Software-Based | Hardware-Supported |
|---|---|---|
| Encryption | CPU executes algorithm | Crypto accelerator |
| Key storage | Software-managed memory | Protected hardware mechanisms |
| Firmware verification | Software verification | Secure-boot chain |
| Random generation | Pseudorandom algorithm | Hardware RNG support |
| Flash protection | Software access control | Flash-encryption mechanisms |

Hardware support can improve:

- performance,
- key isolation,
- resistance to some software attacks,
- boot-time trust,
- predictability of security operations.

However, hardware support does not automatically make an application secure.

---

# 🔑 6. Cryptography

Cryptography protects information by transforming data using mathematical algorithms and keys.

The two major encryption categories are:

```text
Cryptography
     │
     ├──────────────┐
     ▼              ▼
 Symmetric       Asymmetric
 Encryption      Cryptography
     │              │
 Same Key       Public/Private
```

Cryptographic systems also commonly use:

- cryptographic hashes,
- message authentication codes,
- digital signatures,
- secure random numbers.

---

# 🔒 7. Symmetric Encryption

In **symmetric encryption**, the same secret key is used for encryption and decryption.

Let:

- $P$ = plaintext,
- $K$ = secret key,
- $E$ = encryption operation,
- $C$ = ciphertext.

Encryption is:

$$
C=E_K(P)
$$

Decryption is:

$$
P=D_K(C)
$$

The communication model is:

```text
             Shared Secret Key K
             ────────────────
                    │
                    ▼
Plaintext ──► Encryption ──► Ciphertext
                                │
                                ▼
                           Decryption
                                │
                                ▼
                            Plaintext
```

---

# 🔐 8. AES

The **Advanced Encryption Standard (AES)** is a widely used symmetric block cipher.

AES supports key sizes of:

- 128 bits,
- 192 bits,
- 256 bits.

AES operates on:

$$
128\text{-bit blocks}
$$

AES can be used as part of secure communication, encrypted storage, and other cryptographic protocols.

---

# ⚡ 9. Advantages of Symmetric Encryption

Symmetric cryptography is generally computationally efficient.

Advantages include:

- relatively high speed,
- efficient implementation,
- suitable for large amounts of data,
- appropriate for resource-constrained systems.

A simplified comparison is:

```text
Symmetric Encryption

Low/Moderate Computational Cost
            │
            ▼
High Data Throughput
            │
            ▼
Suitable for Bulk Encryption
```

---

# ⚠️ 10. Symmetric Key Distribution Problem

The major challenge is securely distributing the shared secret key.

Suppose Device A and Device B need to communicate:

```text
Device A                       Device B
   │                              │
   └──── Shared Secret K ─────────┘
```

Both devices must already possess or securely establish:

$$
K
$$

If the key is exposed:

$$
\boxed{
K_{\text{compromised}}
\Rightarrow
\text{Protected Communication at Risk}
}
$$

This is one reason asymmetric cryptography and key-agreement protocols are important.

---

# 🔑 11. Asymmetric Cryptography

Asymmetric cryptography uses a **key pair**:

$$
(K_{\text{private}},K_{\text{public}})
$$

The private key must remain secret.

The public key can be distributed.

```text
        Key Pair
           │
     ┌─────┴─────┐
     ▼           ▼
Public Key    Private Key
Shareable      Secret
```

Common applications include:

- digital signatures,
- authentication,
- key establishment,
- certificates.

---

# 🔐 12. Public and Private Keys

For encryption schemes that support public-key encryption, the conceptual process is:

```text
Sender

Plaintext
   │
   ▼
Recipient Public Key
   │
   ▼
Encryption
   │
   ▼
Ciphertext
   │
   ▼
Recipient Private Key
   │
   ▼
Decryption
   │
   ▼
Plaintext
```

In practice, protocols often use asymmetric cryptography to establish or protect session keys and symmetric encryption for bulk data.

---

# ✍️ 13. Digital Signatures

Digital signatures are especially important in embedded systems because they can authenticate firmware.

A simplified signing process is:

```text
Firmware
   │
   ▼
Cryptographic Hash
   │
   ▼
Digest
   │
   ▼
Signing Operation
using Private Key
   │
   ▼
Digital Signature
```

Verification:

```text
Firmware + Signature
        │
        ▼
Public-Key Verification
        │
        ├── Valid ─────► Trusted Origin / Integrity Check Passes
        │
        └── Invalid ───► Reject
```

---

# 🧮 14. Cryptographic Hash Functions

A hash function transforms arbitrary-length input into a fixed-size digest.

$$
h=H(M)
$$

where:

- $M$ = message,
- $H$ = hash function,
- $h$ = digest.

Conceptually:

```text
Firmware
   │
   ▼
SHA-256
   │
   ▼
256-bit Digest
```

A good cryptographic hash is designed to make it computationally infeasible to:

- reconstruct the original message from its digest,
- find another message with the same digest,
- deliberately produce two different messages with the same digest.

---

# 🔍 15. Integrity Verification Using Hashes

Suppose the original firmware has:

$$
h_1=H(F)
$$

After transmission or storage:

$$
h_2=H(F')
$$

If:

$$
h_1=h_2
$$

the data is consistent with the original hash value.

If:

$$
h_1\neq h_2
$$

the firmware has changed.

However, a plain hash alone does not authenticate who supplied the firmware. Secure boot therefore normally uses authenticated signatures or equivalent trust mechanisms.

---

# 🔏 16. Message Authentication Code

A **Message Authentication Code (MAC)** can provide integrity and authenticity using a shared secret.

Conceptually:

$$
T=\operatorname{MAC}_K(M)
$$

The receiver recomputes the MAC using the same secret key.

One widely used construction is HMAC:

$$
\operatorname{HMAC}(K,M)
$$

This is different from a hardware **multiply-accumulate (MAC)** operation.

---

# 🆚 17. Symmetric vs. Asymmetric Cryptography

| Characteristic | Symmetric | Asymmetric |
|---|---|---|
| Keys | Shared secret | Public/private pair |
| Relative speed | Faster | Generally slower |
| Bulk encryption | Excellent | Usually not preferred |
| Key distribution | Challenging | Easier public-key distribution |
| Digital signatures | No | Yes |
| Common examples | AES | RSA, ECC |

In practical secure systems, both approaches are often combined.

---

# 🔄 18. Hybrid Cryptography

A typical secure communication architecture is:

```text
Asymmetric Cryptography
        │
        ▼
Authentication /
Key Establishment
        │
        ▼
Session Key
        │
        ▼
Symmetric Encryption
        │
        ▼
High-Speed Protected Data
```

This approach is used in protocols such as TLS.

---

# 🌐 19. TLS and ESP32

A typical ESP32 Internet application may use:

```text
ESP32
  │
  ▼
Wi-Fi
  │
  ▼
TLS
  │
  ▼
HTTPS / MQTT over TLS
  │
  ▼
Server / Broker
```

TLS can provide:

- server authentication,
- optional client authentication,
- confidentiality,
- message integrity.

---

# 🔐 20. Hardware Cryptographic Acceleration

Many ESP32-family devices provide hardware support for selected cryptographic operations.

Depending on the particular chip, hardware may accelerate operations associated with algorithms such as:

- AES,
- SHA,
- RSA,
- HMAC,
- digital-signature operations.

Hardware acceleration can reduce CPU load and improve cryptographic performance.

The exact accelerator set depends on the ESP32 variant.

---

# 🎲 21. Random Number Generation

Cryptography requires high-quality random or unpredictable values for operations such as:

- key generation,
- nonces,
- challenges,
- initialization values,
- protocol handshakes.

Conceptually:

```text
Entropy Sources
      │
      ▼
Random Number Mechanism
      │
      ▼
Cryptographic Values
```

Weak randomness can compromise otherwise strong cryptographic algorithms.

---

# 🚀 22. What Is Secure Boot?

**Secure boot** is a mechanism designed to prevent unauthorized firmware from executing during the device boot process.

Without secure boot:

```text
Power On
   │
   ▼
Load Firmware
   │
   ▼
Execute
```

An attacker who successfully replaces firmware may cause malicious code to execute.

With secure boot:

```text
Power On
   │
   ▼
Trusted Boot Stage
   │
   ▼
Verify Next Software Stage
   │
   ├── Valid ─────► Continue Boot
   │
   └── Invalid ───► Reject / Stop
```

---

# 🔗 23. Chain of Trust

Secure boot creates a **chain of trust**.

```text
Hardware Trust Anchor
        │
        ▼
Boot ROM / Trusted Initial Code
        │
        ▼
Bootloader Verification
        │
        ▼
Application Verification
        │
        ▼
Trusted Application
```

Conceptually:

$$
T_0\rightarrow T_1\rightarrow T_2\rightarrow T_3
$$

where each trusted stage authenticates the next stage before transferring control.

---

# 🧱 24. Secure Boot Principle

Let:

- $F$ = firmware image,
- $\sigma$ = firmware signature,
- $K_{\text{pub}}$ = trusted public-key information.

The device performs a verification operation conceptually represented as:

$$
V(K_{\text{pub}},F,\sigma)
$$

If:

$$
V=1
$$

the firmware is accepted.

If:

$$
V=0
$$

the firmware is rejected.

---

# ✍️ 25. Firmware Signing

Firmware signing normally occurs in a controlled development or release environment.

```text
Developer / Build System
        │
        ▼
Firmware Binary
        │
        ▼
Hash / Signature Process
        │
        ▼
Signing Private Key
        │
        ▼
Signed Firmware
```

The signing private key should not be embedded in ordinary deployed devices.

The device instead contains or derives trusted verification information.

---

# 🔑 26. Why the Private Signing Key Is Critical

If an attacker obtains the firmware-signing private key, the attacker may be able to create malicious firmware that passes signature verification.

Therefore:

$$
\boxed{
\text{Private Signing Key} =
\text{Critical Security Asset}
}
$$

Production systems may protect signing keys using:

- hardware security modules,
- isolated signing servers,
- strict access controls,
- controlled release pipelines.

---

# 🔒 27. Flash Encryption

Secure boot and flash encryption address different security objectives.

### Secure Boot

Primarily protects:

$$
\boxed{
\text{Firmware Authenticity and Integrity}
}
$$

### Flash Encryption

Primarily protects data stored in external flash from straightforward offline reading.

```text
Application / Data
       │
       ▼
Encryption Mechanism
       │
       ▼
External Flash
       │
       ▼
Encrypted Stored Content
```

The exact ESP32 flash-encryption behavior depends on the chip and security configuration.

---

# 🆚 28. Secure Boot vs. Flash Encryption

| Feature | Secure Boot | Flash Encryption |
|---|---|---|
| Main goal | Prevent unauthorized firmware execution | Protect stored flash contents |
| Authenticity | Yes | Not its primary function |
| Confidentiality | Not primary goal | Yes |
| Boot verification | Yes | No |
| Protect firmware from simple flash reading | Not by itself | Yes |

A stronger embedded-security architecture commonly uses both.

---

# 🧱 29. Root of Trust

A **Root of Trust (RoT)** is a foundational trusted component upon which other security mechanisms depend.

Conceptually:

```text
              Applications
                   ▲
                   │
              Secure Services
                   ▲
                   │
              Secure Boot
                   ▲
                   │
            Root of Trust
```

The Root of Trust should be sufficiently protected because compromise at this layer can undermine higher-level protections.

---

# 🌳 30. Root of Trust Analogy

A useful analogy is a tree:

```text
              Applications
                 /   \
                /     \
          Secure Services
               / \
              /   \
          Secure Boot
               │
               │
          Root of Trust
```

If the root cannot be trusted, trust in everything built above it becomes questionable.

---

# 🔑 31. Hardware Root of Trust

A hardware-based Root of Trust can rely on immutable or hardware-protected mechanisms such as:

- ROM boot code,
- one-time-programmable security configuration,
- device-specific key material,
- secure key derivation,
- hardware cryptographic functions.

For ESP32 devices, the precise implementation varies by chip generation and model.

---

# 💾 32. eFuse-Based Security Configuration

ESP32-family devices use **eFuses** for one-time-programmable configuration and security-related settings.

Conceptually:

```text
eFuse
  │
  ├── Security Configuration
  ├── Boot Security Settings
  ├── Key-Related Material / Digests
  ├── Device Configuration
  └── Feature Restrictions
```

An eFuse bit, once permanently programmed, is generally not reversible in the normal sense.

Therefore, security provisioning must be performed carefully.

---

# ⚠️ 33. Importance of eFuse Configuration

Incorrect permanent security configuration can:

- prevent normal programming,
- change debug availability,
- make firmware updates difficult,
- prevent the system from booting,
- permanently alter device behavior.

For teaching laboratories, students should first study secure-boot and eFuse workflows without irreversibly programming security settings unless appropriate development hardware and recovery procedures are available.

---

# 🔐 34. Key Storage

Cryptographic keys should not simply be placed as readable strings in source code.

Avoid designs such as:

```cpp
const char* secretKey = "MY_SECRET_KEY_123";
```

because firmware extraction or source exposure can reveal the key.

Better approaches may involve:

- hardware-backed key protection,
- secure provisioning,
- protected storage,
- encrypted storage,
- derived keys,
- secure elements where appropriate.

---

# 🗝️ 35. Key Lifecycle

Security must consider the complete key lifecycle:

```text
Key Generation
      │
      ▼
Provisioning
      │
      ▼
Storage
      │
      ▼
Use
      │
      ▼
Rotation / Update
      │
      ▼
Revocation / Retirement
```

Protecting only the encryption algorithm is insufficient if the key lifecycle is weak.

---

# 🧩 36. Device Identity

A connected embedded device should have a trustworthy identity.

Conceptually:

```text
Device
  │
  ├── Unique Identity
  ├── Device Credential
  └── Authentication Key
```

The identity may be associated with:

- device certificates,
- public/private key pairs,
- provisioned credentials,
- hardware-backed secrets.

---

# 📜 37. Digital Certificates

Certificates associate an identity with a public key.

A simplified certificate contains:

```text
Certificate
   │
   ├── Subject
   ├── Public Key
   ├── Issuer
   ├── Validity
   └── CA Signature
```

Certificates are widely used in TLS-based authentication.

---

# 🏛️ 38. Public Key Infrastructure

A simplified PKI structure is:

```text
Trusted Root CA
      │
      ▼
Intermediate CA
      │
      ▼
Device / Server Certificate
      │
      ▼
Authenticated Connection
```

The system ultimately depends on trusted root certificate information.

---

# 🧠 39. Secure Firmware Update

Secure systems must remain secure after deployment.

A secure update process can be:

```text
Firmware Server
      │
      ▼
Signed Firmware
      │
      ▼
Encrypted / Authenticated Channel
      │
      ▼
ESP32
      │
      ▼
Verify Firmware
      │
      ├── Valid ───► Install
      └── Invalid ─► Reject
```

---

# 🔄 40. OTA Firmware Updates

ESP32 applications frequently use **Over-the-Air (OTA)** updates.

Benefits include:

- remote bug fixes,
- feature deployment,
- security patches,
- fleet maintenance.

However, insecure OTA can become an attack vector.

OTA updates should therefore incorporate appropriate:

- transport security,
- firmware authenticity checks,
- version management,
- rollback policy.

---

# ⏪ 41. Anti-Rollback Protection

Suppose:

```text
Version 1.0 → Vulnerable
Version 2.0 → Security Fix
```

An attacker may attempt:

```text
Version 2.0
     │
     ▼
Downgrade
     │
     ▼
Version 1.0
```

Anti-rollback mechanisms are designed to prevent installation of firmware older than an allowed security version.

Conceptually:

$$
V_{\text{new}}
\ge
V_{\text{minimum allowed}}
$$

---

# 🐞 42. Debug Interfaces

Debugging interfaces are extremely useful during development.

Examples include:

- JTAG,
- UART,
- USB debug facilities on applicable devices.

However, unrestricted production debug access may expose:

- memory,
- firmware,
- registers,
- system state.

Security configuration may therefore restrict debugging in production deployments.

---

# ⚖️ 43. Development vs. Production Security

### Development

```text
Debugging Enabled
Flexible Firmware Loading
Easy Recovery
Extensive Logging
```

### Production

```text
Authenticated Firmware
Restricted Debugging
Protected Keys
Controlled Updates
Reduced Sensitive Logging
```

Security configuration should match the device lifecycle stage.

---

# 🧨 44. Threat Model

Security design should begin by identifying:

1. What must be protected?
2. Who may attack it?
3. What capabilities might the attacker have?
4. What are the consequences of compromise?

Example:

| Asset | Threat | Possible Protection |
|---|---|---|
| Firmware | Modification | Secure boot |
| Stored firmware/data | Offline extraction | Flash encryption |
| Network data | Eavesdropping | TLS |
| Device identity | Credential theft | Protected key storage |
| Firmware update | Malicious update | Signed OTA |
| Debug port | Unauthorized access | Production restrictions |

---

# 🧅 45. Defense in Depth

A single security mechanism is rarely sufficient.

A stronger design uses multiple layers:

```text
Layer 5 ─ Application Authentication
Layer 4 ─ TLS / Secure Protocol
Layer 3 ─ Secure Firmware Update
Layer 2 ─ Secure Boot
Layer 1 ─ Flash / Key Protection
Layer 0 ─ Hardware Root of Trust
```

This is known as:

$$
\boxed{
\text{Defense in Depth}
}
$$

---

# 🔐 46. Example ESP32 Secure IoT Architecture

```text
             Cloud Server
                  │
                  │ TLS
                  ▼
             Wi-Fi Network
                  │
                  ▼
        ┌──────────────────┐
        │      ESP32       │
        │                  │
        │ TLS Client       │
        │                  │
        │ Secure OTA       │
        │                  │
        │ Secure Boot      │
        │                  │
        │ Flash Protection │
        │                  │
        │ Root of Trust    │
        └────────┬─────────┘
                 │
                 ▼
              Sensor
```

---

# 📡 47. Secure MQTT

An insecure architecture may use:

```text
ESP32
  │
  ▼
MQTT
  │
  ▼
Broker
```

A more secure architecture may use:

```text
ESP32
  │
  ▼
MQTT over TLS
  │
  ▼
Authenticated Broker
```

The common URI convention is:

```text
mqtts://
```

rather than unsecured MQTT transport.

---

# 🌐 48. Secure REST Communication

Instead of:

```text
http://server/api
```

a secure application generally uses HTTPS:

```text
https://server/api
```

Architecture:

```text
ESP32
  │
  ▼
TLS
  │
  ▼
HTTPS
  │
  ▼
REST API
```

---

# 🔍 49. Certificate Verification

Using TLS encryption without correctly authenticating the remote server can leave the system vulnerable to impersonation or man-in-the-middle attacks.

The ESP32 should validate the server using appropriate trust information such as:

```text
Trusted CA
    │
    ▼
Server Certificate
    │
    ▼
Verification
    │
    ├── Trusted ───► Continue
    └── Invalid ───► Reject
```

---

# 🚨 50. Man-in-the-Middle Attack

An insecure connection may allow:

```text
ESP32 ───────► Server
       ▲
       │
    Attacker
```

or:

```text
ESP32
  │
  ▼
Attacker
  │
  ▼
Server
```

Properly configured authenticated TLS is designed to mitigate this type of attack.

---

# 🧪 51. Replay Attack

An attacker may capture a valid message:

```text
ESP32 ─── "OPEN" ───► Server
```

and later replay it:

```text
Attacker ─── "OPEN" ───► Server
```

Countermeasures can include:

- nonces,
- sequence numbers,
- timestamps where appropriate,
- authenticated protocols,
- session-specific cryptographic state.

---

# 🎲 52. Nonce

A nonce is a value intended for one-time use within a particular cryptographic context.

$$
\boxed{
\text{Nonce} =
\text{Number Used Once}
}
$$

Conceptually:

```text
Message
+
Nonce
+
Authentication
```

can help protocols detect or prevent replay, depending on how the protocol is designed.

---

# ⏱️ 53. Security and Performance

Security consumes embedded resources.

A simplified timing model is:

$$
T_{\text{secure}} =
T_{\text{application}}
+
T_{\text{crypto}}
+
T_{\text{verification}}
+
T_{\text{protocol}}
$$

Similarly, memory usage may increase:

$$
M_{\text{secure}} =
M_{\text{application}}
+
M_{\text{TLS}}
+
M_{\text{certificates}}
+
M_{\text{crypto}}
$$

Security should therefore be considered during system design rather than added only at the end.

---

# ⚡ 54. Hardware Acceleration and Performance

Suppose software AES takes:

$$
T_{\text{SW}}
$$

and hardware-accelerated AES takes:

$$
T_{\text{HW}}
$$

A simple speedup metric is:

$$
S=
\frac{T_{\text{SW}}}
{T_{\text{HW}}}
$$

If:

$$
T_{\text{SW}}=10~\text{ms}
$$

and:

$$
T_{\text{HW}}=2~\text{ms}
$$

then:

$$
S=5
$$

or a theoretical measured speedup of:

$$
5\times
$$

for that particular test.

---

# 💾 55. Security and Memory Constraints

TLS and certificate processing can require substantial RAM on MCU-class systems.

Memory may be needed for:

```text
TLS Session
   │
   ├── Network Buffers
   ├── Certificates
   ├── Crypto Context
   ├── Session Keys
   └── Protocol State
```

Therefore:

$$
M_{\text{available}}
>
M_{\text{application}}
+
M_{\text{security}}
$$

must hold with suitable margin.

---

# 🧠 56. Security and FreeRTOS

A secure ESP32 application may use separate tasks:

```text
Sensor Task
     │
     ▼
Processing Task
     │
     ▼
Secure Communication Queue
     │
     ▼
TLS / Network Task
```

This prevents slow network cryptography from unnecessarily blocking timing-critical sensor operations.

---

# 🔐 57. Root of Trust to Cloud

A complete trust path can be represented as:

```text
Hardware Root of Trust
        │
        ▼
Secure Boot
        │
        ▼
Authenticated Firmware
        │
        ▼
Protected Device Identity
        │
        ▼
TLS Authentication
        │
        ▼
Trusted Cloud Service
```

This connects hardware security to system-level IoT security.

---

# 🤖 58. Security for TinyML

TinyML applications introduce additional assets:

- ML model parameters,
- sensor datasets,
- inference outputs,
- model updates.

A secure TinyML system may use:

```text
Root of Trust
      │
      ▼
Secure Boot
      │
      ▼
Trusted TinyML Firmware
      │
      ▼
Sensor Data
      │
      ▼
ML Inference
      │
      ▼
Prediction
      │
      ▼
TLS
      │
      ▼
Cloud / Digital Twin
```

---

# 🧠 59. Model Integrity

If an attacker modifies model weights:

$$
W\rightarrow W'
$$

the resulting predictions may change:

$$
f(x;W)\neq f(x;W')
$$

Therefore, model integrity may be as important as application-code integrity when the model is stored as part of the firmware or external data.

---

# 🪞 60. Secure Digital Twin Architecture

A secure digital-twin system can be:

```text
Physical System
      │
      ▼
Sensor
      │
      ▼
ESP32
      │
      ├── Root of Trust
      ├── Secure Boot
      ├── Protected Credentials
      └── TinyML
             │
             ▼
          TLS/MQTT
             │
             ▼
        Edge / Cloud
             │
             ▼
        Digital Twin
```

This helps protect the path from physical sensing to digital representation.

---

# 🧪 61. Security Experiment 1 — Hash Integrity

Students can compute a SHA-256 digest for a message:

```text
"ESP32 Security"
```

Then modify it:

```text
"ESP32 security"
```

Compare:

$$
H(M_1)
$$

with:

$$
H(M_2)
$$

A small change in the input should produce a substantially different digest.

---

# 🧪 62. Security Experiment 2 — Symmetric Encryption

Conceptually:

```text
Plaintext
   │
   ▼
AES Encryption
   │
   ▼
Ciphertext
   │
   ▼
AES Decryption
   │
   ▼
Recovered Plaintext
```

Students should verify:

$$
D_K(E_K(P))=P
$$

and discuss why key management is critical.

---

# 🧪 63. Security Experiment 3 — TLS

Compare:

```text
HTTP
```

and:

```text
HTTPS
```

for an ESP32 REST client.

Measure:

| Metric | HTTP | HTTPS |
|---|---:|---:|
| Connection time | | |
| Request latency | | |
| Free RAM | | |
| Response time | | |

Discuss the security-performance trade-off.

---

# 🧪 64. Security Experiment 4 — Firmware Integrity

Students can conceptually compare:

```text
Original Firmware
       │
       ▼
SHA-256
       │
       ▼
Digest A
```

with:

```text
Modified Firmware
       │
       ▼
SHA-256
       │
       ▼
Digest B
```

If:

$$
A\neq B
$$

the modification is detected by the hash comparison.

Then discuss why authentication/signatures are still needed.

---

# 🧪 65. Security Experiment 5 — Secure Boot Study

Without permanently programming security eFuses, investigate the secure-boot workflow:

```text
Firmware
   │
   ▼
Build
   │
   ▼
Sign
   │
   ▼
Verify
   │
   ▼
Boot Decision
```

Identify:

- signing key,
- verification key or digest,
- firmware image,
- signature,
- trust anchor.

---

# ⚠️ 66. Safe Laboratory Practice

Secure boot and flash-encryption experiments require care because some configurations can be irreversible.

For introductory laboratories:

> Use simulation, documentation analysis, disposable development boards, or reversible development configurations before permanently programming security settings.

Never use production credentials in student experiments.

---

# 📊 67. Security Evaluation Table

Students can evaluate an embedded system using:

| Security Requirement | Mechanism | Implemented? |
|---|---|:-:|
| Firmware authenticity | Secure boot | |
| Stored-data confidentiality | Flash encryption | |
| Network confidentiality | TLS | |
| Server authentication | Certificate verification | |
| Device authentication | Device credential | |
| Secure updates | Signed OTA | |
| Rollback resistance | Anti-rollback | |
| Key protection | Protected storage | |
| Debug protection | Production configuration | |

---

# 🔎 68. Common Security Mistakes

Examples include:

1. hard-coding passwords in source code,
2. transmitting credentials using plaintext protocols,
3. disabling certificate verification,
4. accepting unsigned firmware,
5. leaving production debug access unrestricted,
6. using weak or predictable random values,
7. reusing keys across unrelated products,
8. storing private keys in unprotected memory,
9. ignoring firmware rollback,
10. failing to update vulnerable firmware.

---

# 🛡️ 69. Secure Design Principles

A secure ESP32 design should follow principles such as:

### Least Privilege

Give each component only the permissions it needs.

### Defense in Depth

Use multiple independent security layers.

### Secure by Default

Start from secure settings rather than relying on users to enable security later.

### Minimize Attack Surface

Disable unnecessary:

- services,
- ports,
- interfaces,
- debug functions.

### Fail Securely

When authentication or verification fails, avoid automatically proceeding with sensitive operations.

---

# 🔄 70. Security Lifecycle

Security is not a one-time configuration.

```text
Design
  │
  ▼
Threat Modeling
  │
  ▼
Implementation
  │
  ▼
Testing
  │
  ▼
Provisioning
  │
  ▼
Deployment
  │
  ▼
Monitoring
  │
  ▼
Security Updates
  │
  ▼
Device Retirement
```

Security must be maintained throughout the device lifecycle.

---

# 📐 71. Security Risk

A simplified risk model can be expressed as:

$$
R =
P_{\text{attack}}
\times
I_{\text{impact}}
$$

where:

- $P_{\text{attack}}$ = likelihood or probability of successful attack,
- $I_{\text{impact}}$ = impact of compromise.

A more practical qualitative model may consider:

$$
\boxed{
\text{Risk} =
f(
\text{Threat},
\text{Vulnerability},
\text{Likelihood},
\text{Impact}
)
}
$$

---

# 🧪 72. Exercise 1 — Identify Assets

For an ESP32 smart-home controller, identify five important assets.

Example categories:

- firmware,
- Wi-Fi credentials,
- device private key,
- sensor data,
- actuator commands.

For each asset, identify one possible threat.

---

# 🧪 73. Exercise 2 — Symmetric vs. Asymmetric

Complete:

| Requirement | Symmetric | Asymmetric |
|---|:-:|:-:|
| Bulk data encryption | ✓ | |
| Digital signature | | ✓ |
| Shared secret required | ✓ | |
| Public/private pair | | ✓ |
| High-throughput encryption | ✓ | |

Explain why TLS uses a hybrid approach.

---

# 🧪 74. Exercise 3 — Secure Boot Chain

Draw and explain:

```text
Hardware
   │
   ▼
Boot ROM
   │
   ▼
Bootloader
   │
   ▼
Application
```

Identify where verification occurs and what establishes initial trust.

---

# 🧪 75. Exercise 4 — Root of Trust

Explain why:

$$
\boxed{
\text{Root of Trust}
\rightarrow
\text{Chain of Trust}
\rightarrow
\text{Trusted Application}
}
$$

is stronger than allowing arbitrary application firmware to execute directly.

---

# 🧪 76. Exercise 5 — Threat Analysis

Consider:

```text
ESP32
  │
  ▼
Wi-Fi
  │
  ▼
MQTT Broker
```

Identify threats involving:

- eavesdropping,
- impersonation,
- firmware modification,
- credential theft,
- replay attacks.

Propose one or more countermeasures for each.

---

# 🚀 77. Advanced Exercise — Secure ESP32 IoT Node

Design:

```text
Sensor
   │
   ▼
ESP32
   │
   ├── Hardware Root of Trust
   ├── Secure Boot
   ├── Flash Protection
   ├── Protected Credentials
   ├── TinyML
   └── MQTT over TLS
           │
           ▼
       MQTT Broker
           │
           ▼
       Digital Twin
```

Evaluate:

1. firmware authenticity,
2. firmware confidentiality,
3. device authentication,
4. server authentication,
5. network confidentiality,
6. key protection,
7. OTA security,
8. rollback protection.

---

# 💬 78. Discussion Questions

1. What is hardware security?
2. What are confidentiality, integrity, and availability?
3. What is authentication?
4. How does authorization differ from authentication?
5. What is symmetric encryption?
6. Why is AES suitable for embedded systems?
7. What is the symmetric key-distribution problem?
8. What is asymmetric cryptography?
9. What are public and private keys?
10. What is a digital signature?
11. What is a cryptographic hash?
12. Why is a plain hash insufficient for firmware authentication?
13. What is HMAC?
14. What is hybrid cryptography?
15. Why does TLS use both asymmetric and symmetric cryptography?
16. What is secure boot?
17. What is a chain of trust?
18. What is firmware signing?
19. Why must the signing private key be protected?
20. What is flash encryption?
21. How does flash encryption differ from secure boot?
22. What is a Root of Trust?
23. Why is a hardware Root of Trust important?
24. What are eFuses?
25. Why must eFuse configuration be performed carefully?
26. What is secure key storage?
27. What is a key lifecycle?
28. What is device identity?
29. What is a digital certificate?
30. What is PKI?
31. What is secure OTA?
32. What is anti-rollback protection?
33. Why can debug interfaces become security risks?
34. What is defense in depth?
35. What is a threat model?
36. Why should ESP32 use HTTPS instead of HTTP?
37. Why should MQTT use TLS for sensitive deployments?
38. What is certificate verification?
39. What is a replay attack?
40. What is a nonce?

---

# 🧾 79. Expected Learning Outcomes

After studying this theory, students should be able to:

- explain the role of hardware security in MCU systems,
- describe confidentiality, integrity, and availability,
- distinguish authentication from authorization,
- explain symmetric encryption,
- explain asymmetric cryptography,
- compare AES, RSA, and ECC conceptually,
- explain digital signatures,
- describe cryptographic hash functions,
- explain HMAC,
- describe hybrid cryptography,
- explain TLS-based embedded communication,
- explain hardware cryptographic acceleration,
- describe the importance of random-number generation,
- explain secure boot,
- describe a chain of trust,
- explain firmware signing,
- distinguish secure boot from flash encryption,
- define Root of Trust,
- explain hardware-based trust anchors,
- describe the role of ESP32 eFuses,
- explain secure key storage,
- describe the key lifecycle,
- explain device identity and certificates,
- describe secure OTA,
- explain anti-rollback protection,
- identify security risks from debug interfaces,
- create a basic threat model,
- apply defense-in-depth principles,
- design a secure ESP32 IoT architecture.

---

# 🔑 80. Key Concepts

### CIA Security Model

$$
\boxed{
\text{Security} =
\text{Confidentiality}
+
\text{Integrity}
+
\text{Availability}
}
$$

### Symmetric Encryption

$$
\boxed{
C=E_K(P)
}
$$

$$
\boxed{
P=D_K(C)
}
$$

### Hash Function

$$
\boxed{
h=H(M)
}
$$

### Message Authentication

$$
\boxed{
T=\operatorname{MAC}_K(M)
}
$$

### Secure Boot Verification

$$
\boxed{
V(K_{\text{pub}},F,\sigma) =
\begin{cases}
1,&\text{Accept}\\
0,&\text{Reject}
\end{cases}
}
$$

### Chain of Trust

$$
\boxed{
T_0
\rightarrow
T_1
\rightarrow
T_2
\rightarrow
T_3
}
$$

### Root of Trust

$$
\boxed{
\text{Root of Trust}
\rightarrow
\text{Secure Boot}
\rightarrow
\text{Trusted Firmware}
}
$$

### Defense in Depth

$$
\boxed{
\text{Security} =
\sum
\text{Multiple Protective Layers}
}
$$

---

# 🎯 81. Summary

Security in ESP32-based embedded systems must extend from the hardware foundation to firmware, network communication, and cloud connectivity.

A strong security architecture can be summarized as:

```text
Hardware Root of Trust
        │
        ▼
Secure Boot
        │
        ▼
Authenticated Firmware
        │
        ▼
Protected Keys and Data
        │
        ▼
Secure Communication
        │
        ▼
Authenticated Cloud / Broker
```

The four central concepts are:

$$
\boxed{
\text{Secure Embedded System} =
\text{Hardware Security}
+
\text{Cryptography}
+
\text{Secure Boot}
+
\text{Root of Trust}
}
$$

**Symmetric cryptography**, such as AES, provides efficient protection for bulk data.

**Asymmetric cryptography**, including RSA- and ECC-based mechanisms, supports authentication, digital signatures, and secure key establishment.

**Secure boot** prevents unauthorized firmware from becoming part of the trusted execution chain by verifying software before execution.

**Root of Trust** establishes the foundation from which all higher-level trust is derived.

For an ESP32 IoT or TinyML system, these mechanisms can be combined with:

- flash encryption,
- protected key storage,
- TLS,
- signed OTA updates,
- anti-rollback mechanisms,
- secure provisioning,
- restricted production debugging.

The resulting security principle is:

$$
\boxed{
\text{Trust the Hardware}
\rightarrow
\text{Verify the Firmware}
\rightarrow
\text{Protect the Keys}
\rightarrow
\text{Authenticate Communication}
}
$$

This provides the foundation for secure **IoT nodes, cyber-physical systems, TinyML devices, smart sensors, industrial controllers, and digital-twin endpoints** using ESP32-class microcontrollers.

---

# 📘 References

1. Espressif Systems, *ESP-IDF Programming Guide — Security*.  
2. Espressif Systems, *ESP32 Series Datasheets and Technical Reference Manuals*.  
3. Espressif Systems, *Secure Boot Documentation*.  
4. Espressif Systems, *Flash Encryption Documentation*.  
5. Espressif Systems, *Security Overview and Hardware Security Features*.  
6. NIST, *Advanced Encryption Standard (AES), FIPS PUB 197*.  
7. NIST, *Secure Hash Standard (SHS), FIPS PUB 180-4*.  
8. NIST, *Digital Signature Standard (DSS), FIPS PUB 186*.  
9. NIST, *Recommendation for Key Management, SP 800-57*.  
10. W. Stallings, *Cryptography and Network Security: Principles and Practice*, Pearson.  
11. J. Katz and Y. Lindell, *Introduction to Modern Cryptography*, CRC Press.  
12. FreeRTOS, *FreeRTOS Kernel and Security Documentation*.

---

