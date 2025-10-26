
Name: Ayesha Khalid
Registration Number:** 1248

Overview

This mini project is based on controlling multiple LEDs through push-buttons with an OLED display for feedback.
Each button press changes the operating mode of LEDs, while the second button resets the system to default.
The program is written for the ESP32 board and tested using Wokwi simulation.

---

## Working Concept

* **Button 1** → shifts through different operating modes.
* **Button 2** → brings all LEDs back to the OFF state.
* **OLED** → shows which mode is active.

**LED Modes:**

1. **Mode 0:** All LEDs OFF
2. **Mode 1:** Alternate blink
3. **Mode 2:** All LEDs ON
4. **Mode 3:** LED3 runs in PWM fade pattern

---

##  Pin Mapping

| Component            | GPIO Pin | Function / Description           |
| -------------------- | -------- | -------------------------------- |
| LED1                 | 4        | Normal LED for blink modes       |
| LED2                 | 2        | Secondary LED for alternate mode |
| LED3                 | 18       | PWM fade effect                  |
| Button – Mode Select | 32       | Switch to next mode              |
| Button – Reset       | 33       | Turns off all LEDs               |
| OLED SDA             | 21       | Data line                        |
| OLED SCL             | 22       | Clock line                       |


---

##  Project Media

All screenshots and demo clip are placed in the `Screenshots-videos` folder:

* Mode 0 – All OFF
* Mode 1 – Alternate Blink
* Mode 2 – Both ON
* Mode 3 – Fade Effect

---
 ## How to Run the Code
* Open the assignment folder in VS Code.
* Make sure the PlatformIO and Wokwi extensions are installed.
* Click Build to compile the code.
* Open the diagram.json file – the Wokwi simulator will launch a     automatically.
* Press the Run Simulation button and observe LED mode changes on the circuit and OLED.
* Try both buttons to test mode switching and reset functionality.
---

## Conclusion

The project successfully shows multi-mode LED control with clear OLED feedback using two push-buttons.
It verifies correct use of digital I/O, PWM, and I2C display control on the ESP32 platform.
