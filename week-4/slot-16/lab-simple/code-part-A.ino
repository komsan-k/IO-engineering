/*
 * LAB — Part A
 * AES-256 Encryption and Decryption on ESP32
 *
 * Mode:
 *   AES-256-CBC
 *
 * Demonstrates:
 *   1. Define a 256-bit key
 *   2. Prepare plaintext
 *   3. Add PKCS#7 padding
 *   4. Encrypt
 *   5. Display ciphertext in HEX
 *   6. Decrypt
 *   7. Remove padding
 *   8. Verify recovered plaintext
 *
 * Board:
 *   ESP32 / ESP32-S3
 */

#include <Arduino.h>
#include "mbedtls/aes.h"


// =====================================================
// AES Configuration
// =====================================================

#define AES_KEY_SIZE       32
#define AES_BLOCK_SIZE     16
#define MAX_BUFFER_SIZE    128


// =====================================================
// AES-256 Key
// 32 bytes = 256 bits
// =====================================================

const unsigned char aesKey[AES_KEY_SIZE] = {

  0x10, 0x21, 0x32, 0x43,
  0x54, 0x65, 0x76, 0x87,
  0x98, 0xA9, 0xBA, 0xCB,
  0xDC, 0xED, 0xFE, 0x0F,

  0x01, 0x12, 0x23, 0x34,
  0x45, 0x56, 0x67, 0x78,
  0x89, 0x9A, 0xAB, 0xBC,
  0xCD, 0xDE, 0xEF, 0xF0
};


// =====================================================
// Initialization Vector
// 16 bytes = 128 bits
// =====================================================

const unsigned char initialIV[AES_BLOCK_SIZE] = {

  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};


// =====================================================
// Print binary data as HEX
// =====================================================

void printHex(
  const unsigned char *data,
  size_t length
)
{
  for (
    size_t i = 0;
    i < length;
    i++
  )
  {
    if (
      data[i] < 0x10
    )
    {
      Serial.print(
        "0"
      );
    }

    Serial.print(
      data[i],
      HEX
    );

    Serial.print(
      " "
    );
  }

  Serial.println();
}


// =====================================================
// PKCS#7 Padding
// =====================================================

size_t addPKCS7Padding(
  const unsigned char *input,
  size_t inputLength,
  unsigned char *output
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
      paddingLength;
  }

  return (
    inputLength
    +
    paddingLength
  );
}


// =====================================================
// Remove PKCS#7 Padding
// =====================================================

bool removePKCS7Padding(
  unsigned char *buffer,
  size_t inputLength,
  size_t &outputLength
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
    paddingLength > AES_BLOCK_SIZE
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

  buffer[
    outputLength
  ] =
    '\0';

  return true;
}


// =====================================================
// AES-256-CBC Encryption
// =====================================================

bool encryptAES256CBC(
  const unsigned char *plaintext,
  size_t plaintextLength,
  unsigned char *ciphertext,
  size_t &ciphertextLength
)
{
  unsigned char padded[
    MAX_BUFFER_SIZE
  ];

  ciphertextLength =
    addPKCS7Padding(
      plaintext,
      plaintextLength,
      padded
    );

  if (
    ciphertextLength
    >
    MAX_BUFFER_SIZE
  )
  {
    return false;
  }


  // CBC modifies the IV internally.
  // Always use a working copy.

  unsigned char iv[
    AES_BLOCK_SIZE
  ];

  memcpy(
    iv,
    initialIV,
    AES_BLOCK_SIZE
  );


  mbedtls_aes_context aes;

  mbedtls_aes_init(
    &aes
  );


  int result =
    mbedtls_aes_setkey_enc(
      &aes,
      aesKey,
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
// AES-256-CBC Decryption
// =====================================================

bool decryptAES256CBC(
  const unsigned char *ciphertext,
  size_t ciphertextLength,
  unsigned char *plaintext,
  size_t &plaintextLength
)
{
  if (
    ciphertextLength
    %
    AES_BLOCK_SIZE
    !=
    0
  )
  {
    return false;
  }


  unsigned char iv[
    AES_BLOCK_SIZE
  ];

  memcpy(
    iv,
    initialIV,
    AES_BLOCK_SIZE
  );


  mbedtls_aes_context aes;

  mbedtls_aes_init(
    &aes
  );


  int result =
    mbedtls_aes_setkey_dec(
      &aes,
      aesKey,
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
// Run one AES experiment
// =====================================================

void runAESExperiment(
  const char *message
)
{
  unsigned char ciphertext[
    MAX_BUFFER_SIZE
  ];

  unsigned char recovered[
    MAX_BUFFER_SIZE + 1
  ];


  size_t plaintextLength =
    strlen(
      message
    );

  size_t ciphertextLength =
    0;

  size_t recoveredLength =
    0;


  Serial.println();
  Serial.println(
    "===================================="
  );

  Serial.println(
    "AES-256-CBC Experiment"
  );

  Serial.println(
    "===================================="
  );


  Serial.print(
    "Plaintext: "
  );

  Serial.println(
    message
  );


  Serial.print(
    "Plaintext length: "
  );

  Serial.print(
    plaintextLength
  );

  Serial.println(
    " bytes"
  );


  // -------------------------------------------------
  // Encryption
  // -------------------------------------------------

  bool encrypted =
    encryptAES256CBC(
      (const unsigned char*) message,
      plaintextLength,
      ciphertext,
      ciphertextLength
    );


  if (
    !encrypted
  )
  {
    Serial.println(
      "Encryption failed."
    );

    return;
  }


  Serial.println();

  Serial.print(
    "Ciphertext length: "
  );

  Serial.print(
    ciphertextLength
  );

  Serial.println(
    " bytes"
  );


  Serial.println(
    "Ciphertext HEX:"
  );

  printHex(
    ciphertext,
    ciphertextLength
  );


  // -------------------------------------------------
  // Decryption
  // -------------------------------------------------

  bool decrypted =
    decryptAES256CBC(
      ciphertext,
      ciphertextLength,
      recovered,
      recoveredLength
    );


  if (
    !decrypted
  )
  {
    Serial.println(
      "Decryption failed."
    );

    return;
  }


  Serial.println();

  Serial.print(
    "Recovered plaintext: "
  );

  Serial.println(
    (char*) recovered
  );


  Serial.print(
    "Recovered length: "
  );

  Serial.print(
    recoveredLength
  );

  Serial.println(
    " bytes"
  );


  // -------------------------------------------------
  // Verification
  // -------------------------------------------------

  bool same =
    strcmp(
      message,
      (char*) recovered
    )
    ==
    0;


  Serial.println();

  Serial.print(
    "Verification: "
  );


  if (
    same
  )
  {
    Serial.println(
      "PASS"
    );
  }
  else
  {
    Serial.println(
      "FAIL"
    );
  }


  Serial.println(
    "===================================="
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
    1500
  );


  Serial.println();
  Serial.println(
    "ESP32 AES-256 Security Lab"
  );


  // Lab example 1

  runAESExperiment(
    "Hello ESP32"
  );


  // Lab example 2

  runAESExperiment(
    "temperature=27.5"
  );


  // Lab example 3

  runAESExperiment(
    "humidity=68"
  );
}


// =====================================================
// Loop
// =====================================================

void loop()
{
}
