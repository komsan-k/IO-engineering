# Tutorial: Hand Pose Estimation Using Python

In this tutorial, you will use a webcam to detect a hand and display its **21 landmarks**. You will also display the index fingertip’s position.

## 1. Learning Objectives

After completing this tutorial, you should be able to:

- Capture webcam images using OpenCV.
- Detect hand landmarks using MediaPipe.
- Draw the hand skeleton.
- Extract a fingertip’s coordinates.

## 2. What You Need

- A computer with Python installed.
- A built-in or USB webcam.
- An internet connection for installing packages and downloading the model.

The processing runs on your computer. An ESP32 is not required for this tutorial.

## 3. Install the Python Libraries

Open a terminal or Windows Command Prompt:

```bash
python -m pip install mediapipe opencv-python
```

| Library | Purpose |
|---|---|
| OpenCV | Read webcam frames and display images |
| MediaPipe | Estimate hand landmarks |

## 4. Download the Hand Model

Open the [MediaPipe Hand Landmarker model page](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker#models) and download **HandLandmarker (full)**.

Save the model as `hand_landmarker.task`. MediaPipe uses this pretrained model to detect the hand’s 21 landmarks.

Create a folder containing these two files:

```text
hand_pose_tutorial/
    hand_pose.py
    hand_landmarker.task
```

## 5. Create the Python Program

Save the following code as `hand_pose.py`:

```python
from pathlib import Path
import time

import cv2
import mediapipe as mp


# Find the model beside this Python script.
MODEL_PATH = Path(__file__).resolve().with_name("hand_landmarker.task")

# Pairs of landmark numbers that form the hand skeleton.
CONNECTIONS = [
    (0, 1), (1, 2), (2, 3), (3, 4),         # Thumb
    (0, 5), (5, 6), (6, 7), (7, 8),         # Index
    (5, 9), (9, 10), (10, 11), (11, 12),    # Middle
    (9, 13), (13, 14), (14, 15), (15, 16),  # Ring
    (13, 17), (17, 18), (18, 19), (19, 20), # Little
    (0, 17),                                # Palm
]


def main():
    if not MODEL_PATH.is_file():
        raise FileNotFoundError(
            "Place hand_landmarker.task beside hand_pose.py"
        )

    # Configure detection for one hand.
    options = mp.tasks.vision.HandLandmarkerOptions(
        base_options=mp.tasks.BaseOptions(
            model_asset_path=str(MODEL_PATH)
        ),
        running_mode=mp.tasks.vision.RunningMode.VIDEO,
        num_hands=1,
    )

    camera = cv2.VideoCapture(0)

    if not camera.isOpened():
        camera.release()
        raise RuntimeError("Cannot open webcam.")

    previous_timestamp = -1

    try:
        with mp.tasks.vision.HandLandmarker.create_from_options(
            options
        ) as detector:

            while True:
                success, frame = camera.read()
                if not success:
                    print("Cannot read webcam image.")
                    break

                # Mirror the image.
                frame = cv2.flip(frame, 1)
                height, width = frame.shape[:2]

                # OpenCV uses BGR; MediaPipe expects RGB.
                rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                image = mp.Image(
                    image_format=mp.ImageFormat.SRGB,
                    data=rgb,
                )

                # VIDEO mode requires increasing timestamps.
                timestamp = max(
                    time.monotonic_ns() // 1_000_000,
                    previous_timestamp + 1,
                )
                previous_timestamp = timestamp

                result = detector.detect_for_video(image, timestamp)
                message = "No hand detected"

                for hand in result.hand_landmarks:
                    # Convert normalized coordinates to image pixels.
                    points = [
                        (int(p.x * width), int(p.y * height))
                        for p in hand
                    ]

                    # Draw the hand skeleton.
                    for start, end in CONNECTIONS:
                        cv2.line(
                            frame, points[start], points[end],
                            (0, 255, 0), 2,
                        )

                    # Draw and number all landmarks.
                    for number, point in enumerate(points):
                        cv2.circle(frame, point, 4, (0, 0, 255), -1)
                        cv2.putText(
                            frame, str(number),
                            (point[0] + 5, point[1] - 5),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.4, (255, 255, 255), 1,
                        )

                    # Landmark 8 is the index fingertip.
                    x, y = points[8]
                    cv2.circle(frame, (x, y), 10, (0, 255, 255), 2)
                    message = f"Index fingertip: x={x}, y={y}"

                cv2.putText(
                    frame, message, (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6, (0, 255, 255), 2,
                )

                cv2.imshow("Hand Pose - Press Q to quit", frame)

                if cv2.waitKey(1) & 0xFF == ord("q"):
                    break

    finally:
        camera.release()
        cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
```

This uses MediaPipe’s video mode to process successive webcam frames. Video mode can use tracking between frames to reduce repeated palm detection.

## 6. Run the Program

Open a terminal inside your tutorial folder:

```bash
python hand_pose.py
```

Hold your hand in front of the webcam with your fingers visible. Press **Q** while the camera window is selected to quit.

## 7. Understand the Results

You should see:

- **Red dots:** detected hand landmarks.
- **Green lines:** connections forming the hand skeleton.
- **White numbers:** landmark IDs.
- **Yellow circle:** index fingertip.
- **Text:** index fingertip coordinates in pixels.

| Landmark ID | Position |
|---|---|
| 0 | Wrist |
| 4 | Thumb tip |
| 8 | Index fingertip |
| 12 | Middle fingertip |
| 16 | Ring fingertip |
| 20 | Little fingertip |

Image coordinates start at the **top-left corner**. Moving right increases x, and moving down increases y.

The conversion from normalized coordinates to pixels is:

$$
x_{\text{pixel}} = x_{\text{normalized}} \times \text{image width}
$$

$$
y_{\text{pixel}} = y_{\text{normalized}} \times \text{image height}
$$

## 8. Simple Student Exercises

| Exercise | Modification |
|---|---|
| Track the thumb | Change `points[8]` to `points[4]` and update the message label |
| Track the middle fingertip | Change `points[8]` to `points[12]` and update the message label |
| Detect two hands | Change `num_hands=1` to `num_hands=2`; the text will show the last hand processed |
| Detect a pinch | Calculate the distance between landmarks 4 and 8 |

## 9. Troubleshooting

| Problem | Suggested action |
|---|---|
| Camera does not open | Check camera permissions and close other applications using the camera |
| Wrong camera selected | Try `cv2.VideoCapture(1)` |
| Model file not found | Place `hand_landmarker.task` beside `hand_pose.py` |
| Tracking is poor | Improve lighting and keep the entire hand visible |

Run the program locally on a computer with a webcam and desktop display.

**Validation note:** The code has not been tested with a physical webcam here.

## References

- [MediaPipe Hand Landmarker overview and models](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker)
- [MediaPipe Hand Landmarker Python guide](https://ai.google.dev/edge/mediapipe/solutions/vision/hand_landmarker/python)
