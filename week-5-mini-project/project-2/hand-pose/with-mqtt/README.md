# Hand Pose Estimation with MQTT Messaging Using HiveMQ

This version keeps the hand skeleton display and **publishes hand-pose data to HiveMQ using MQTT**.

Each message includes the detection status, index fingertip position, and all 21 landmarks.

## 1. Install the Libraries

```bash
python -m pip install mediapipe opencv-python "paho-mqtt>=2,<3"
```

Keep `hand_landmarker.task` in the same folder as the Python script. Download **HandLandmarker (full)** from the [official MediaPipe model page](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker#models).

## 2. MQTT Settings

This example uses HiveMQ’s shared public broker. No account is required. Use a unique topic for your experiment; public-broker messages are accessible to others. Port 1883 uses unencrypted MQTT.

| Setting | Value |
|---|---|
| Broker | `broker.hivemq.com` |
| Port | `1883` |
| Topic | `psu/kanjanasit/handpose/lab01/data` |
| Publish interval | 0.5 seconds |
| Message format | JSON |

## 3. Complete Python Code

Save as `hand_pose_mqtt.py`.

The MQTT connection runs in the background using Paho’s version 2 callback API, so the webcam can continue operating while the client connects or reconnects.

```python
from pathlib import Path
import json
import time
import uuid

import cv2
import mediapipe as mp
import paho.mqtt.client as mqtt


# ---------- Configuration ----------
MODEL_PATH = Path(__file__).resolve().with_name("hand_landmarker.task")
CAMERA_INDEX = 0  # Try 1 if the wrong camera is selected.

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883

# Change this topic to a unique value for your experiment.
MQTT_TOPIC = "psu/kanjanasit/handpose/lab01/data"
PUBLISH_INTERVAL = 0.5  # Seconds: at most 2 messages per second.

CLIENT_ID = f"handpose-{uuid.uuid4().hex[:12]}"

CONNECTIONS = [
    (0, 1), (1, 2), (2, 3), (3, 4),         # Thumb
    (0, 5), (5, 6), (6, 7), (7, 8),         # Index
    (5, 9), (9, 10), (10, 11), (11, 12),    # Middle
    (9, 13), (13, 14), (14, 15), (15, 16),  # Ring
    (13, 17), (17, 18), (18, 19), (19, 20), # Little
    (0, 17),                                # Palm
]


# ---------- MQTT callbacks ----------
def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print(f"MQTT connected: {MQTT_BROKER}")
        print(f"Publishing to: {MQTT_TOPIC}")
    else:
        print(f"MQTT connection rejected: {reason_code}")


def on_disconnect(
    client, userdata, disconnect_flags, reason_code, properties
):
    print(f"MQTT disconnected: {reason_code}")


def on_connect_fail(client, userdata):
    print("MQTT connection failed; retrying in the background.")


# ---------- Main program ----------
def main():
    if not MODEL_PATH.is_file():
        raise FileNotFoundError(
            "Place hand_landmarker.task beside this Python script."
        )

    camera = cv2.VideoCapture(CAMERA_INDEX)

    if not camera.isOpened():
        camera.release()
        raise RuntimeError(
            "Cannot open webcam. Check permissions and camera index."
        )

    client = mqtt.Client(
        callback_api_version=mqtt.CallbackAPIVersion.VERSION2,
        client_id=CLIENT_ID,
        protocol=mqtt.MQTTv311,
    )

    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_connect_fail = on_connect_fail
    client.reconnect_delay_set(min_delay=1, max_delay=30)

    options = mp.tasks.vision.HandLandmarkerOptions(
        base_options=mp.tasks.BaseOptions(
            model_asset_path=str(MODEL_PATH)
        ),
        running_mode=mp.tasks.vision.RunningMode.VIDEO,
        num_hands=1,
    )

    previous_timestamp = -1
    last_publish = 0.0
    sequence = 0

    try:
        # Connect without blocking the camera loop.
        client.connect_async(MQTT_BROKER, MQTT_PORT, keepalive=60)
        client.loop_start()

        with mp.tasks.vision.HandLandmarker.create_from_options(
            options
        ) as detector:

            while True:
                success, frame = camera.read()
                if not success:
                    print("Cannot read webcam image.")
                    break

                frame = cv2.flip(frame, 1)
                height, width = frame.shape[:2]

                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                image = mp.Image(
                    image_format=mp.ImageFormat.SRGB,
                    data=rgb,
                )

                timestamp = max(
                    time.monotonic_ns() // 1_000_000,
                    previous_timestamp + 1,
                )
                previous_timestamp = timestamp

                result = detector.detect_for_video(image, timestamp)

                # Default message when no hand is visible.
                payload = {
                    "device_id": CLIENT_ID,
                    "timestamp_ms": time.time_ns() // 1_000_000,
                    "hand_detected": False,
                    "message": "NO_HAND",
                    "image_width": width,
                    "image_height": height,
                    "index_tip": None,
                    "landmarks": [],
                }

                display_message = "No hand detected"

                if result.hand_landmarks:
                    hand = result.hand_landmarks[0]

                    points = [
                        (int(p.x * width), int(p.y * height))
                        for p in hand
                    ]

                    # Draw the skeleton.
                    for start, end in CONNECTIONS:
                        cv2.line(
                            frame, points[start], points[end],
                            (0, 255, 0), 2,
                        )

                    # Draw numbered landmarks.
                    for number, point in enumerate(points):
                        cv2.circle(frame, point, 4, (0, 0, 255), -1)
                        cv2.putText(
                            frame, str(number),
                            (point[0] + 5, point[1] - 5),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.4, (255, 255, 255), 1,
                        )

                    x, y = points[8]
                    cv2.circle(frame, (x, y), 10, (0, 255, 255), 2)
                    display_message = f"Index fingertip: x={x}, y={y}"

                    payload.update({
                        "hand_detected": True,
                        "message": "HAND_DETECTED",
                        "index_tip": {
                            "x_px": x,
                            "y_px": y,
                            "x": round(hand[8].x, 5),
                            "y": round(hand[8].y, 5),
                            "z": round(hand[8].z, 5),
                        },
                        "landmarks": [
                            {
                                "id": i,
                                "x": round(p.x, 5),
                                "y": round(p.y, 5),
                                "z": round(p.z, 5),
                            }
                            for i, p in enumerate(hand)
                        ],
                    })

                # Publish the latest result at a controlled rate.
                now = time.monotonic()

                if (
                    client.is_connected()
                    and now - last_publish >= PUBLISH_INTERVAL
                ):
                    sequence += 1
                    payload["sequence"] = sequence

                    info = client.publish(
                        MQTT_TOPIC,
                        json.dumps(payload),
                        qos=0,
                        retain=False,
                    )

                    if info.rc == mqtt.MQTT_ERR_SUCCESS:
                        print(
                            f"MQTT queued #{sequence}: "
                            f"{payload['message']} | "
                            f"index_tip={payload['index_tip']}"
                        )
                    else:
                        print(f"MQTT publish failed: {info.rc}")

                    last_publish = now

                mqtt_status = (
                    "CONNECTED" if client.is_connected()
                    else "CONNECTING / RECONNECTING"
                )

                cv2.putText(
                    frame, display_message, (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 255, 255), 2,
                )
                cv2.putText(
                    frame, f"MQTT: {mqtt_status}", (10, 60),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.55, (0, 255, 255), 2,
                )

                cv2.imshow("Hand Pose MQTT - Press Q to quit", frame)

                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break

    finally:
        camera.release()
        cv2.destroyAllWindows()
        client.disconnect()
        client.loop_stop()


if __name__ == "__main__":
    main()
```

## 4. Run the Program

```bash
python hand_pose_mqtt.py
```

Make sure the webcam shows a live image first. If it shows a crossed-out camera icon, enable the camera using its privacy switch or manufacturer settings and check Windows camera permissions.

## 5. Receive the Messages

In an MQTT client, connect to `broker.hivemq.com` on port `1883` and subscribe to:

```text
psu/kanjanasit/handpose/lab01/data
```

If Mosquitto clients are installed, use:

```bash
mosquitto_sub -h broker.hivemq.com -p 1883 -t "psu/kanjanasit/handpose/lab01/data" -v
```

An illustrative message, with the landmarks shortened, is:

```json
{
  "device_id": "handpose-a1b2c3d4e5f6",
  "timestamp_ms": 1792645200000,
  "hand_detected": true,
  "message": "HAND_DETECTED",
  "image_width": 640,
  "image_height": 480,
  "index_tip": {
    "x_px": 320,
    "y_px": 120,
    "x": 0.5,
    "y": 0.25,
    "z": -0.03
  },
  "landmarks": [],
  "sequence": 1
}
```

The actual detected-hand message contains all **21 landmarks**. Coordinates describe the mirrored image; `z` is relative depth. When the hand disappears, the program sends `NO_HAND` with `index_tip: null` and an empty landmarks list.

This sends coordinates and status messages; webcam images remain on your computer. With QoS 0, delivery is best effort. A successful queue result does not confirm receipt by a subscriber.

**Validation note:** The webcam-to-broker workflow has not been tested here.

## References

- [HiveMQ public MQTT broker](https://www.hivemq.com/mqtt/public-mqtt-broker/)
- [Eclipse Paho MQTT Python documentation](https://eclipse.dev/paho/clients/python/docs/)
- [MediaPipe Hand Landmarker Python guide](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker/python)
