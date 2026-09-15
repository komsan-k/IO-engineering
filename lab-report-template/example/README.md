# Simple Lab Report

## 1. Achievement

Briefly describe what you learned from the experiments.

- Configured ESP32 **GPIO input and output**.
- Read the state of a push button through the GPIO input.
- Controlled an LED using button input.
- Applied **bare-metal/register-level GPIO programming**.

---

## 2. Experiment 5 — Display Button State

### Result

Record the observed button states.

| Button State | GPIO Logic | Serial Monitor Output |
|---|---|---|
| Released | | |
| Pressed | | |

**Screenshot:**  
Insert a screenshot of the Serial Monitor showing the button states.

### Discussion

1. What GPIO value is observed when the button is **released**?
2. What GPIO value is observed when the button is **pressed**?
3. How does the program read the button state from the GPIO register?

---

## 3. Experiment 9 — Button Toggle Program

### Result

Press the button several times and record the LED state.

| Button Press | LED State |
|---:|---|
| Initial | |
| 1 | |
| 2 | |
| 3 | |
| 4 | |

**Screenshot/Photo:**  
Insert a screenshot or photo showing the button-controlled LED.

### Discussion

1. How does each button press change the LED state?
2. Why is the previous button state required for toggle operation?
3. What happens if switch bouncing occurs?

---

## 4. Conclusion

Briefly summarize what you learned about:

- ESP32 GPIO input and output
- Register-level button reading
- LED control using a push button
- Button-state and toggle detection
