import requests
import json
import time
import os
import random
from datetime import datetime


# --------------------------------
# HTTP Server
# --------------------------------

serverURL = "https://httpbin.org/post"


# --------------------------------
# Refresh Interval
# --------------------------------

REFRESH_TIME = 5


# --------------------------------
# HTTP Header
# --------------------------------

headers = {
    "Content-Type": "application/json"
}


# --------------------------------
# Clear Terminal
# --------------------------------

def clear_screen():

    if os.name == "nt":
        os.system("cls")

    else:
        os.system("clear")


# ========================================
# HTTP POST Live Monitor
# ========================================

try:

    while True:

        # --------------------------------
        # Generate Random Sensor Data
        # --------------------------------

        temperature = round(
            random.uniform(25.0, 35.0),
            1
        )

        humidity = round(
            random.uniform(50.0, 90.0),
            1
        )


        # --------------------------------
        # Create JSON Data
        # --------------------------------

        sensorData = {
            "temperature": temperature,
            "humidity": humidity
        }


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
            # Decode Response
            # --------------------------------

            data = response.json()

            receivedData = data.get(
                "json"
            )


            # --------------------------------
            # Refresh Terminal
            # --------------------------------

            clear_screen()


            # --------------------------------
            # Display Monitor
            # --------------------------------

            print("=" * 50)
            print("       HTTP POST LIVE MONITOR")
            print("=" * 50)


            print(
                "\nTime:",
                datetime.now().strftime(
                    "%Y-%m-%d %H:%M:%S"
                )
            )


            print("\nHTTP Method:")
            print("POST")


            print("\nServer:")
            print(serverURL)


            print("\nHTTP Response Code:")
            print(response.status_code)


            # --------------------------------
            # Display Data Sent
            # --------------------------------

            print("\n" + "-" * 50)
            print("DATA SENT")
            print("-" * 50)

            print(
                json.dumps(
                    sensorData,
                    indent=4
                )
            )


            # --------------------------------
            # Display Data Received
            # --------------------------------

            print("\n" + "-" * 50)
            print("DATA RECEIVED BY HTTPBIN")
            print("-" * 50)

            print(
                json.dumps(
                    receivedData,
                    indent=4
                )
            )


            # --------------------------------
            # Sensor Monitor
            # --------------------------------

            if receivedData:

                print("\n" + "-" * 50)
                print("SENSOR MONITOR")
                print("-" * 50)

                print(
                    "Temperature :",
                    receivedData.get(
                        "temperature"
                    ),
                    "C"
                )

                print(
                    "Humidity    :",
                    receivedData.get(
                        "humidity"
                    ),
                    "%"
                )


            # --------------------------------
            # Refresh Information
            # --------------------------------

            print("\n" + "=" * 50)

            print(
                "Refresh every",
                REFRESH_TIME,
                "seconds"
            )

            print(
                "Press Ctrl+C to stop"
            )

            print("=" * 50)


        # --------------------------------
        # HTTP Error
        # --------------------------------

        except requests.exceptions.RequestException as error:

            clear_screen()

            print("HTTP POST failed")

            print(
                "Error:",
                error
            )


        # --------------------------------
        # JSON Error
        # --------------------------------

        except json.JSONDecodeError:

            clear_screen()

            print(
                "Invalid JSON response"
            )


        # --------------------------------
        # Wait Before Next POST
        # --------------------------------

        time.sleep(
            REFRESH_TIME
        )


# ========================================
# Stop Program
# ========================================

except KeyboardInterrupt:

    print()

    print(
        "HTTP POST Monitor stopped."
    )
