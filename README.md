
# 7-Segment Display Controller using Shift Registers and Multiplexing

This project demonstrates how to control three 7-segment displays using shift registers (e.g., 74HC595) and multiplexing. The code enables the displays to show 3-digit numbers with optional decimal points.

## Features

- Display numbers and decimal points on three 7-segment displays.
- Use shift registers to reduce the number of GPIO pins required.
- Multiplexing ensures efficient use of hardware resources.
- Easily update display values dynamically.

## Hardware Setup

1. **Shift Registers**:
   - Connect shift registers in series.
   - Each display uses 8 bits from the shift registers.

2. **7-Segment Displays**:
   - The digit select pins control which digit is active.
   - Decimal points are controlled by the most significant bit in the digit pattern.

3. **Pin Connections**:
   - `SHIFT_DATA_PIN`: Pin 11 (connected to shift register data pin).
   - `SHIFT_LATCH_PIN`: Pin 12 (connected to shift register latch pin).
   - `SHIFT_CLOCK_PIN`: Pin 13 (connected to shift register clock pin).
   - `digitPins`: Pins 3, 4, 6 (control digit selection).

## Software Setup

### Prerequisites

- Arduino IDE installed on your computer.
- A supported microcontroller (e.g., Arduino Uno).

### Code Explanation

1. **Pin Definitions**:
   - Define pins for shift register and digit select.

2. **Display Data**:
   - Digit patterns for numbers 0-9.
   - Manage decimal points and special characters.

3. **Functions**:
   - `formatForDisplay()`: Formats numbers for display.
   - `updateDisplayValue()`: Updates the display value dynamically.
   - `displayFormattedNumbers()`: Handles multiplexing and display updates.

### Uploading the Code

1. Open the Arduino IDE.
2. Copy the code from `7-Segment_Display_Controller.ino`.
3. Connect your Arduino to your computer via USB.
4. Select the appropriate board and port in the IDE.
5. Upload the code.

## Wiring Diagram

Make sure to follow the connections as described in the hardware setup section.

## Usage

1. Power the circuit with the Arduino.
2. The displays will show the initial values.
3. Use the `updateDisplayValue()` function to change the display content dynamically.

## License

This project is open-source and licensed under the MIT License.

---

 by Gustavo Silveira - aka the Nerd Musician
 Learn how to build your MIDI controllers: https://go.musiconerd.com/nerd-musician-pro