/*
 * LAB — Encrypted IoT Sensor Communication using ESP32, AES-256, and MQTT
 * ----------------------------------------------------------------------
 * ESP32 B — Subscriber
 *
 * Functions:
 *   1. Connect to Wi-Fi and MQTT
 *   2. Subscribe to encrypted LDR topic
 *   3. Receive HEX ciphertext
 *   4. Convert HEX -> bytes
 *   5. Decrypt with AES-256-CBC
 *   6. Remove PKCS#7 padding
 *   7. Display recovered "ldr=<value>"
 *
 * Required Arduino library:
 *   PubSubClient
 *
 * IMPORTANT:
 *   Publisher and subscriber MUST use exactly the same
 *   AES key and IV.
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
  "ESP32_LDR_Subscriber";

// =====================================================
// AES Configuration
// =====================================================

#define AES_KEY_SIZE    32
#define AES_BLOCK_SIZE  16
#define MAX_CIPHERTEXT  144
#define MAX_HEX_LENGTH  (MAX_CIPHERTEXT * 2 + 1)
#define MAX_PLAINTEXT   128

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

// =====================================================
// HEX Helpers
// =====================================================

int hexNibble(
  char c
)
{
  if (
    c >= '0'
    &&
    c <= '9'
  )
  {
    return c - '0';
  }

  if (
    c >= 'A'
    &&
    c <= 'F'
  )
  {
    return c - 'A' + 10;
  }

  if (
    c >= 'a'
    &&
    c <= 'f'
  )
  {
    return c - 'a' + 10;
  }

  return -1;
}

bool hexToBytes(
  const char* hex,
  size_t hexLength,
  unsigned char* output,
  size_t outputCapacity,
  size_t& outputLength
)
{
  if (
    hexLength % 2
    !=
    0
  )
  {
    return false;
  }

  outputLength =
    hexLength / 2;

  if (
    outputLength
    >
    outputCapacity
  )
  {
    return false;
  }

  for (
    size_t i = 0;
    i < outputLength;
    i++
  )
  {
    int high =
      hexNibble(
        hex[
          i * 2
        ]
      );

    int low =
      hexNibble(
        hex[
          i * 2 + 1
        ]
      );

    if (
      high < 0
      ||
      low < 0
    )
    {
      return false;
    }

    output[i] =
      (
        unsigned char
      )
      (
        (
          high << 4
        )
        |
        low
      );
  }

  return true;
}

// =====================================================
// PKCS#7 Unpadding
// =====================================================

bool removePKCS7Padding(
  unsigned char* buffer,
  size_t inputLength,
  size_t& outputLength
)
{
  if (
    inputLength == 0
  )
  {
    return false;
  }

  unsigned char paddingLength =
    buffer[
      inputLength - 1
    ];

  if (
    paddingLength == 0
    ||
    paddingLength
    >
    AES_BLOCK_SIZE
    ||
    paddingLength
    >
    inputLength
  )
  {
    return false;
  }

  for (
    size_t i = 0;
    i < paddingLength;
    i++
  )
  {
    if (
      buffer[
        inputLength - 1 - i
      ]
      !=
      paddingLength
    )
    {
      return false;
    }
  }

  outputLength =
    inputLength
    -
    paddingLength;

  return true;
}

// =====================================================
// AES-256-CBC Decryption
// =====================================================

bool decryptAES256CBC(
  const unsigned char* ciphertext,
  size_t ciphertextLength,
  unsigned char* plaintext,
  size_t plaintextCapacity,
  size_t& plaintextLength
)
{
  if (
    ciphertextLength == 0
    ||
    ciphertextLength
    %
    AES_BLOCK_SIZE
    !=
    0
    ||
    ciphertextLength
    >
    plaintextCapacity
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
    mbedtls_aes_setkey_dec(
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
      MBEDTLS_AES_DECRYPT,
      ciphertextLength,
      iv,
      ciphertext,
      plaintext
    );

  mbedtls_aes_free(
    &aes
  );

  if (
    result != 0
  )
  {
    return false;
  }

  return removePKCS7Padding(
    plaintext,
    ciphertextLength,
    plaintextLength
  );
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
// MQTT Callback
// =====================================================

void mqttCallback(
  char* topic,
  byte* payload,
  unsigned int length
)
{
  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.print(
    "Topic: "
  );

  Serial.println(
    topic
  );

  // -------------------------------------------------
  // Plain topic — comparison only
  // -------------------------------------------------

  if (
    strcmp(
      topic,
      TOPIC_PLAIN
    )
    ==
    0
  )
  {
    Serial.print(
      "Plain MQTT payload: "
    );

    for (
      unsigned int i = 0;
      i < length;
      i++
    )
    {
      Serial.write(
        payload[i]
      );
    }

    Serial.println();

    Serial.println(
      "========================================"
    );

    return;
  }

  // -------------------------------------------------
  // Encrypted topic
  // -------------------------------------------------

  if (
    strcmp(
      topic,
      TOPIC_ENCRYPTED
    )
    !=
    0
  )
  {
    return;
  }

  if (
    length == 0
    ||
    length
    >=
    MAX_HEX_LENGTH
  )
  {
    Serial.println(
      "Invalid encrypted payload length"
    );

    return;
  }

  char encryptedHex[
    MAX_HEX_LENGTH
  ];

  memcpy(
    encryptedHex,
    payload,
    length
  );

  encryptedHex[
    length
  ] =
    '\0';

  Serial.print(
    "Encrypted HEX: "
  );

  Serial.println(
    encryptedHex
  );

  unsigned char ciphertext[
    MAX_CIPHERTEXT
  ];

  size_t ciphertextLength =
    0;

  if (
    !hexToBytes(
      encryptedHex,
      length,
      ciphertext,
      sizeof(ciphertext),
      ciphertextLength
    )
  )
  {
    Serial.println(
      "HEX decoding failed"
    );

    Serial.println(
      "========================================"
    );

    return;
  }

  unsigned char plaintext[
    MAX_PLAINTEXT + 1
  ];

  size_t plaintextLength =
    0;

  unsigned long decryptionStart =
    micros();

  bool decrypted =
    decryptAES256CBC(
      ciphertext,
      ciphertextLength,
      plaintext,
      MAX_PLAINTEXT,
      plaintextLength
    );

  unsigned long decryptionTime =
    micros()
    -
    decryptionStart;

  if (
    !decrypted
  )
  {
    Serial.println(
      "AES decryption / padding validation FAILED"
    );

    Serial.println(
      "Possible causes:"
    );

    Serial.println(
      "- Wrong AES key"
    );

    Serial.println(
      "- Wrong IV"
    );

    Serial.println(
      "- Modified ciphertext"
    );

    Serial.println(
      "========================================"
    );

    return;
  }

  plaintext[
    plaintextLength
  ] =
    '\0';

  Serial.print(
    "Recovered data : "
  );

  Serial.println(
    (
      char*
    ) plaintext
  );

  Serial.print(
    "Cipher bytes   : "
  );

  Serial.println(
    ciphertextLength
  );

  Serial.print(
    "Plain bytes    : "
  );

  Serial.println(
    plaintextLength
  );

  Serial.print(
    "Decryption time: "
  );

  Serial.print(
    decryptionTime
  );

  Serial.println(
    " us"
  );

  // Optional: parse "ldr=<number>"
  int ldrValue =
    -1;

  if (
    sscanf(
      (
        char*
      ) plaintext,
      "ldr=%d",
      &ldrValue
    )
    ==
    1
  )
  {
    Serial.print(
      "Parsed LDR     : "
    );

    Serial.println(
      ldrValue
    );
  }

  Serial.println(
    "========================================"
  );
}

// =====================================================
// MQTT Connection
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

    mqttClient.subscribe(
      TOPIC_PLAIN
    );

    mqttClient.subscribe(
      TOPIC_ENCRYPTED
    );

    Serial.print(
      "Subscribed: "
    );

    Serial.println(
      TOPIC_PLAIN
    );

    Serial.print(
      "Subscribed: "
    );

    Serial.println(
      TOPIC_ENCRYPTED
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

  connectWiFi();

  mqttClient.setServer(
    MQTT_BROKER,
    MQTT_PORT
  );

  mqttClient.setCallback(
    mqttCallback
  );

  mqttClient.setBufferSize(
    512
  );

  ensureMQTTConnection();

  Serial.println();
  Serial.println(
    "ESP32 AES-256 MQTT LDR Subscriber"
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
}
