/*
 * LAB — Encrypted IoT Sensor Communication using ESP32, AES-256, and MQTT
 * ----------------------------------------------------------------------
 * ESP32 A — Publisher
 *
 * Functions:
 *   1. Read LDR from ADC
 *   2. Build plaintext: "ldr=<value>"
 *   3. Encrypt with AES-256-CBC + PKCS#7 padding
 *   4. Convert ciphertext to HEX
 *   5. Publish plaintext and encrypted payload through MQTT
 *
 * Required Arduino library:
 *   PubSubClient
 *
 * ESP32 built-in components:
 *   WiFi
 *   mbedTLS
 *
 * IMPORTANT:
 *   - Replace Wi-Fi and MQTT settings.
 *   - Publisher and subscriber MUST use the same AES key and IV.
 *   - Fixed IV is used only for this teaching lab.
 *   - For production, use a fresh/unique IV and authenticated encryption
 *     such as AES-GCM.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mbedtls/aes.h"

// =====================================================
// User Configuration
// =====================================================

const char* WIFI_SSID =
  "YOUR_WIFI_SSID";

const char* WIFI_PASSWORD =
  "YOUR_WIFI_PASSWORD";

const char* MQTT_BROKER =
  "192.168.1.100";

const uint16_t MQTT_PORT =
  1883;

// Leave empty if broker does not use username/password
const char* MQTT_USERNAME =
  "";

const char* MQTT_PASSWORD =
  "";

const char* TOPIC_PLAIN =
  "iot/lab/ldr/plain";

const char* TOPIC_ENCRYPTED =
  "iot/lab/ldr/encrypted";

const char* MQTT_CLIENT_ID =
  "ESP32_LDR_Publisher";

// Classic ESP32 example.
// Change this to a valid ADC pin for your specific board.
#define LDR_PIN 36

// Publishing interval
const unsigned long PUBLISH_INTERVAL_MS =
  2000;

// =====================================================
// AES Configuration
// =====================================================

#define AES_KEY_SIZE    32
#define AES_BLOCK_SIZE  16
#define MAX_PLAINTEXT   128
#define MAX_CIPHERTEXT  144
#define MAX_HEX_LENGTH  (MAX_CIPHERTEXT * 2 + 1)

// AES-256 key: 32 bytes = 256 bits
const unsigned char AES_KEY[AES_KEY_SIZE] =
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

// AES-CBC IV: 16 bytes
// Fixed IV is for teaching only.
const unsigned char INITIAL_IV[AES_BLOCK_SIZE] =
{
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};

// =====================================================
// Network Objects
// =====================================================

WiFiClient wifiClient;
PubSubClient mqttClient(
  wifiClient
);

unsigned long lastPublishTime =
  0;

// =====================================================
// PKCS#7 Padding
// =====================================================

size_t addPKCS7Padding(
  const unsigned char* input,
  size_t inputLength,
  unsigned char* output,
  size_t outputCapacity
)
{
  size_t paddingLength =
    AES_BLOCK_SIZE
    -
    (
      inputLength
      %
      AES_BLOCK_SIZE
    );

  size_t paddedLength =
    inputLength
    +
    paddingLength;

  if (
    paddedLength
    >
    outputCapacity
  )
  {
    return 0;
  }

  memcpy(
    output,
    input,
    inputLength
  );

  for (
    size_t i = 0;
    i < paddingLength;
    i++
  )
  {
    output[
      inputLength + i
    ] =
      (unsigned char) paddingLength;
  }

  return paddedLength;
}

// =====================================================
// AES-256-CBC Encryption
// =====================================================

bool encryptAES256CBC(
  const unsigned char* plaintext,
  size_t plaintextLength,
  unsigned char* ciphertext,
  size_t ciphertextCapacity,
  size_t& ciphertextLength
)
{
  unsigned char padded[
    MAX_CIPHERTEXT
  ];

  ciphertextLength =
    addPKCS7Padding(
      plaintext,
      plaintextLength,
      padded,
      sizeof(padded)
    );

  if (
    ciphertextLength == 0
    ||
    ciphertextLength
    >
    ciphertextCapacity
  )
  {
    return false;
  }

  unsigned char iv[
    AES_BLOCK_SIZE
  ];

  memcpy(
    iv,
    INITIAL_IV,
    AES_BLOCK_SIZE
  );

  mbedtls_aes_context aes;

  mbedtls_aes_init(
    &aes
  );

  int result =
    mbedtls_aes_setkey_enc(
      &aes,
      AES_KEY,
      256
    );

  if (
    result != 0
  )
  {
    mbedtls_aes_free(
      &aes
    );

    return false;
  }

  result =
    mbedtls_aes_crypt_cbc(
      &aes,
      MBEDTLS_AES_ENCRYPT,
      ciphertextLength,
      iv,
      padded,
      ciphertext
    );

  mbedtls_aes_free(
    &aes
  );

  return (
    result == 0
  );
}

// =====================================================
// Binary -> HEX
// =====================================================

bool bytesToHex(
  const unsigned char* data,
  size_t length,
  char* output,
  size_t outputCapacity
)
{
  const char HEX_DIGITS[] =
    "0123456789ABCDEF";

  size_t needed =
    length * 2 + 1;

  if (
    outputCapacity
    <
    needed
  )
  {
    return false;
  }

  for (
    size_t i = 0;
    i < length;
    i++
  )
  {
    output[
      i * 2
    ] =
      HEX_DIGITS[
        (
          data[i] >> 4
        )
        &
        0x0F
      ];

    output[
      i * 2 + 1
    ] =
      HEX_DIGITS[
        data[i]
        &
        0x0F
      ];
  }

  output[
    length * 2
  ] =
    '\0';

  return true;
}

// =====================================================
// Wi-Fi
// =====================================================

void connectWiFi()
{
  Serial.print(
    "Connecting to Wi-Fi"
  );

  WiFi.mode(
    WIFI_STA
  );

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
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

  Serial.print(
    "IP address: "
  );

  Serial.println(
    WiFi.localIP()
  );
}

// =====================================================
// MQTT
// =====================================================

bool connectMQTTOnce()
{
  Serial.print(
    "Connecting to MQTT broker..."
  );

  bool connected;

  if (
    strlen(
      MQTT_USERNAME
    )
    >
    0
  )
  {
    connected =
      mqttClient.connect(
        MQTT_CLIENT_ID,
        MQTT_USERNAME,
        MQTT_PASSWORD
      );
  }
  else
  {
    connected =
      mqttClient.connect(
        MQTT_CLIENT_ID
      );
  }

  if (
    connected
  )
  {
    Serial.println(
      "connected"
    );
  }
  else
  {
    Serial.print(
      "failed, state="
    );

    Serial.println(
      mqttClient.state()
    );
  }

  return connected;
}

void ensureMQTTConnection()
{
  while (
    !mqttClient.connected()
  )
  {
    if (
      connectMQTTOnce()
    )
    {
      break;
    }

    delay(
      2000
    );
  }
}

// =====================================================
// Publish One LDR Reading
// =====================================================

void publishLDR()
{
  int ldrValue =
    analogRead(
      LDR_PIN
    );

  char plaintext[
    MAX_PLAINTEXT
  ];

  snprintf(
    plaintext,
    sizeof(plaintext),
    "ldr=%d",
    ldrValue
  );

  unsigned char ciphertext[
    MAX_CIPHERTEXT
  ];

  size_t ciphertextLength =
    0;

  unsigned long encryptionStart =
    micros();

  bool encrypted =
    encryptAES256CBC(
      (
        const unsigned char*
      ) plaintext,
      strlen(
        plaintext
      ),
      ciphertext,
      sizeof(ciphertext),
      ciphertextLength
    );

  unsigned long encryptionTime =
    micros()
    -
    encryptionStart;

  if (
    !encrypted
  )
  {
    Serial.println(
      "AES encryption failed"
    );

    return;
  }

  char encryptedHex[
    MAX_HEX_LENGTH
  ];

  if (
    !bytesToHex(
      ciphertext,
      ciphertextLength,
      encryptedHex,
      sizeof(encryptedHex)
    )
  )
  {
    Serial.println(
      "HEX conversion failed"
    );

    return;
  }

  // Publish plaintext for comparison
  bool plainOK =
    mqttClient.publish(
      TOPIC_PLAIN,
      plaintext
    );

  // Publish encrypted payload
  bool encryptedOK =
    mqttClient.publish(
      TOPIC_ENCRYPTED,
      encryptedHex
    );

  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.print(
    "LDR value       : "
  );

  Serial.println(
    ldrValue
  );

  Serial.print(
    "Plaintext       : "
  );

  Serial.println(
    plaintext
  );

  Serial.print(
    "Ciphertext HEX  : "
  );

  Serial.println(
    encryptedHex
  );

  Serial.print(
    "Plain bytes     : "
  );

  Serial.println(
    strlen(
      plaintext
    )
  );

  Serial.print(
    "Cipher bytes    : "
  );

  Serial.println(
    ciphertextLength
  );

  Serial.print(
    "HEX characters  : "
  );

  Serial.println(
    strlen(
      encryptedHex
    )
  );

  Serial.print(
    "Encryption time : "
  );

  Serial.print(
    encryptionTime
  );

  Serial.println(
    " us"
  );

  Serial.print(
    "Plain MQTT      : "
  );

  Serial.println(
    plainOK
    ?
    "OK"
    :
    "FAILED"
  );

  Serial.print(
    "Encrypted MQTT  : "
  );

  Serial.println(
    encryptedOK
    ?
    "OK"
    :
    "FAILED"
  );

  Serial.println(
    "========================================"
  );
}

// =====================================================
// Setup
// =====================================================

void setup()
{
  Serial.begin(
    115200
  );

  delay(
    1000
  );

  pinMode(
    LDR_PIN,
    INPUT
  );

  connectWiFi();

  mqttClient.setServer(
    MQTT_BROKER,
    MQTT_PORT
  );

  // Tiny payloads, but set explicitly for lab flexibility.
  mqttClient.setBufferSize(
    512
  );

  ensureMQTTConnection();

  Serial.println();
  Serial.println(
    "ESP32 AES-256 MQTT LDR Publisher"
  );
}

// =====================================================
// Loop
// =====================================================

void loop()
{
  if (
    WiFi.status()
    !=
    WL_CONNECTED
  )
  {
    connectWiFi();
  }

  if (
    !mqttClient.connected()
  )
  {
    ensureMQTTConnection();
  }

  mqttClient.loop();

  unsigned long now =
    millis();

  if (
    now
    -
    lastPublishTime
    >=
    PUBLISH_INTERVAL_MS
  )
  {
    lastPublishTime =
      now;

    publishLDR();
  }
}
