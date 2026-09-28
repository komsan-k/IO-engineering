import requests
import json
from datetime import datetime

# --------------------------------
# HTTP Server
# --------------------------------

serverURL = "https://httpbin.org/post"


# --------------------------------
# Sensor Data
# Same data as ESP32
# --------------------------------

sensorData = {
    "temperature": 25.5,
    "humidity": 60.2
}


# --------------------------------
# HTTP Header
# --------------------------------

headers = {
    "Content-Type": "application/json"
}


# --------------------------------
# Display Request
# --------------------------------

print("=" * 50)
print(" HTTP POST MONITOR")
print("=" * 50)

print("\nTime:")
print(datetime.now())

print("\nURL:")
print(serverURL)

print("\nHTTP Method:")
print("POST")

print("\nHeaders:")
print(headers)

print("\nJSON Data:")
print(
    json.dumps(
        sensorData,
        indent=4
    )
)


# --------------------------------
# Send HTTP POST
# --------------------------------

try:

    response = requests.post(
        serverURL,
        json=sensorData,
        headers=headers,
        timeout=10
    )


    # --------------------------------
    # Monitor HTTP Response
    # --------------------------------

    print("\n" + "=" * 50)
    print(" HTTP RESPONSE")
    print("=" * 50)

    print(
        "\nHTTP Response Code:",
        response.status_code
    )

    print(
        "\nContent-Type:",
        response.headers.get(
            "Content-Type"
        )
    )


    # --------------------------------
    # Decode Server Response
    # --------------------------------

    data = response.json()


    print("\nServer Response:")

    print(
        json.dumps(
            data,
            indent=4
        )
    )


    # --------------------------------
    # Show JSON Received by httpbin
    # --------------------------------

    print("\n" + "=" * 50)
    print(" DATA RECEIVED BY HTTPBIN")
    print("=" * 50)

    receivedData = data.get(
        "json"
    )

    print(
        json.dumps(
            receivedData,
            indent=4
        )
    )


    # --------------------------------
    # Display Sensor Values
    # --------------------------------

    if receivedData:

        print(
            "\nTemperature:",
            receivedData.get(
                "temperature"
            ),
            "C"
        )

        print(
            "Humidity:",
            receivedData.get(
                "humidity"
            ),
            "%"
        )


except requests.exceptions.RequestException as error:

    print("\nHTTP POST failed")

    print(
        "Error:",
        error
    )
