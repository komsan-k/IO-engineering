/*
 * LAB — Part B
 * SHA-256 Integrity Verification on ESP32
 *
 * Demonstrates:
 *   1. Calculate SHA-256 of a message
 *   2. Display the 256-bit digest in HEX
 *   3. Demonstrate the avalanche effect
 *   4. Compare calculated and expected hashes
 *   5. Detect modified data
 *
 * Board:
 *   ESP32 / ESP32-S3
 */

#include <Arduino.h>
#include <mbedtls/sha256.h>


// =====================================================
// Constants
// =====================================================

#define SHA256_LENGTH 32
#define SHA256_HEX_LENGTH 64


// =====================================================
// Convert SHA-256 bytes to HEX string
// =====================================================

String hashToHex(
  const unsigned char *hash
)
{
  String hexString = "";

  for (
    int i = 0;
    i < SHA256_LENGTH;
    i++
  )
  {
    if (
      hash[i] < 0x10
    )
    {
      hexString += "0";
    }

    hexString += String(
      hash[i],
      HEX
    );
  }

  hexString.toUpperCase();

  return hexString;
}


// =====================================================
// Calculate SHA-256
// =====================================================

bool calculateSHA256(
  const char *message,
  unsigned char *outputHash
)
{
  mbedtls_sha256_context ctx;

  mbedtls_sha256_init(
    &ctx
  );


  // SHA-256 = 0
  // SHA-224 = 1

  int result =
    mbedtls_sha256_starts(
      &ctx,
      0
    );

  if (
    result != 0
  )
  {
    mbedtls_sha256_free(
      &ctx
    );

    return false;
  }


  result =
    mbedtls_sha256_update(
      &ctx,
      (const unsigned char*) message,
      strlen(message)
    );

  if (
    result != 0
  )
  {
    mbedtls_sha256_free(
      &ctx
    );

    return false;
  }


  result =
    mbedtls_sha256_finish(
      &ctx,
      outputHash
    );


  mbedtls_sha256_free(
    &ctx
  );


  return (
    result == 0
  );
}


// =====================================================
// Print SHA-256 result
// =====================================================

String printSHA256(
  const char *message
)
{
  unsigned char hash[
    SHA256_LENGTH
  ];


  bool success =
    calculateSHA256(
      message,
      hash
    );


  if (
    !success
  )
  {
    Serial.println(
      "SHA-256 calculation failed."
    );

    return "";
  }


  String hashString =
    hashToHex(
      hash
    );


  Serial.println(
    "----------------------------------------"
  );

  Serial.print(
    "Message : "
  );

  Serial.println(
    message
  );


  Serial.print(
    "Length  : "
  );

  Serial.print(
    strlen(message)
  );

  Serial.println(
    " bytes"
  );


  Serial.println(
    "SHA-256 :"
  );

  Serial.println(
    hashString
  );


  Serial.print(
    "HEX characters: "
  );

  Serial.println(
    hashString.length()
  );


  return hashString;
}


// =====================================================
// Integrity Verification
// =====================================================

bool verifyHash(
  String calculatedHash,
  String expectedHash
)
{
  calculatedHash.toUpperCase();
  expectedHash.toUpperCase();

  return calculatedHash.equals(
    expectedHash
  );
}


// =====================================================
// Experiment 1
// Basic SHA-256
// =====================================================

void experimentBasicSHA256()
{
  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "EXPERIMENT 1 — SHA-256"
  );

  Serial.println(
    "========================================"
  );


  const char *message =
    "ESP32 Secure System";


  printSHA256(
    message
  );
}


// =====================================================
// Experiment 2
// Avalanche Effect
// =====================================================

void experimentAvalancheEffect()
{
  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "EXPERIMENT 2 — Avalanche Effect"
  );

  Serial.println(
    "========================================"
  );


  const char *message1 =
    "ESP32 Secure System";

  const char *message2 =
    "ESP32 Secure system";


  String hash1 =
    printSHA256(
      message1
    );


  String hash2 =
    printSHA256(
      message2
    );


  Serial.println();
  Serial.println(
    "Hash Comparison:"
  );


  Serial.print(
    "Hash 1: "
  );

  Serial.println(
    hash1
  );


  Serial.print(
    "Hash 2: "
  );

  Serial.println(
    hash2
  );


  if (
    hash1 == hash2
  )
  {
    Serial.println(
      "Result: SAME"
    );
  }
  else
  {
    Serial.println(
      "Result: DIFFERENT"
    );

    Serial.println(
      "Avalanche effect demonstrated."
    );
  }
}


// =====================================================
// Experiment 3
// Integrity Verification
// =====================================================

void experimentIntegrityVerification()
{
  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "EXPERIMENT 3 — Integrity Verification"
  );

  Serial.println(
    "========================================"
  );


  const char *originalMessage =
    "Firmware Version 1";


  unsigned char trustedHashBytes[
    SHA256_LENGTH
  ];


  calculateSHA256(
    originalMessage,
    trustedHashBytes
  );


  String expectedHash =
    hashToHex(
      trustedHashBytes
    );


  Serial.println(
    "Trusted original message:"
  );

  Serial.println(
    originalMessage
  );


  Serial.println();

  Serial.println(
    "Expected SHA-256:"
  );

  Serial.println(
    expectedHash
  );


  // -------------------------------------------------
  // Test A — Original data
  // -------------------------------------------------

  Serial.println();
  Serial.println(
    "TEST A — Original Data"
  );


  String calculatedHash1 =
    printSHA256(
      "Firmware Version 1"
    );


  bool valid1 =
    verifyHash(
      calculatedHash1,
      expectedHash
    );


  Serial.print(
    "Verification: "
  );


  if (
    valid1
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


  // -------------------------------------------------
  // Test B — Modified data
  // -------------------------------------------------

  Serial.println();
  Serial.println(
    "TEST B — Modified Data"
  );


  String calculatedHash2 =
    printSHA256(
      "Firmware Version 2"
    );


  bool valid2 =
    verifyHash(
      calculatedHash2,
      expectedHash
    );


  Serial.print(
    "Verification: "
  );


  if (
    valid2
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

    Serial.println(
      "Modified data detected."
    );
  }
}


// =====================================================
// Experiment 4
// Simulated Firmware Verification
// =====================================================

void experimentFirmwareVerification()
{
  Serial.println();
  Serial.println(
    "========================================"
  );

  Serial.println(
    "EXPERIMENT 4 — Firmware Verification"
  );

  Serial.println(
    "========================================"
  );


  const char *trustedFirmware =
    "ESP32_FIRMWARE_IMAGE_V1";


  unsigned char trustedHash[
    SHA256_LENGTH
  ];


  calculateSHA256(
    trustedFirmware,
    trustedHash
  );


  String expectedFirmwareHash =
    hashToHex(
      trustedHash
    );


  Serial.println(
    "Expected firmware hash:"
  );

  Serial.println(
    expectedFirmwareHash
  );


  // -------------------------------------------------
  // Simulated downloaded firmware
  // -------------------------------------------------

  const char *downloadedFirmware =
    "ESP32_FIRMWARE_IMAGE_V1";


  unsigned char downloadedHash[
    SHA256_LENGTH
  ];


  calculateSHA256(
    downloadedFirmware,
    downloadedHash
  );


  String calculatedFirmwareHash =
    hashToHex(
      downloadedHash
    );


  Serial.println();

  Serial.println(
    "Calculated firmware hash:"
  );

  Serial.println(
    calculatedFirmwareHash
  );


  if (
    verifyHash(
      calculatedFirmwareHash,
      expectedFirmwareHash
    )
  )
  {
    Serial.println();

    Serial.println(
      "Firmware integrity OK"
    );

    Serial.println(
      "Firmware accepted."
    );
  }
  else
  {
    Serial.println();

    Serial.println(
      "Firmware verification FAILED"
    );

    Serial.println(
      "Firmware rejected."
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
    1500
  );


  Serial.println();
  Serial.println(
    "ESP32 SHA-256 Integrity Lab"
  );


  experimentBasicSHA256();


  experimentAvalancheEffect();


  experimentIntegrityVerification();


  experimentFirmwareVerification();
}


// =====================================================
// Loop
// =====================================================

void loop()
{
}
