/*
 * 7-Segment Display Controller using Shift Registers and Multiplexing
 *
 * This code controls three 7-segment displays using shift registers (e.g., 74HC595).
 * Each display shows a 3-digit number with optional decimal points.
 * The displays are multiplexed, and data is shifted out to the shift registers.
 *
 * Hardware Setup:
 * - Shift registers are connected in series.
 * - Each display uses 8 bits from the shift registers.
 * - The digit select pins control which digit is currently active.
 * - The decimal point is controlled by the most significant bit (bit 7) in the digit pattern.

 by Gustavo Silveira - aka the Nerd Musician
 Learn how to build your MIDI controllers: https://go.musiconerd.com/nerd-musician-pro
 */

// ----------------------------------------------------------------------------
// Pin Definitions
// ----------------------------------------------------------------------------
#define SHIFT_DATA_PIN 11   // Data pin connected to shift register
#define SHIFT_LATCH_PIN 12  // Latch pin connected to shift register
#define SHIFT_CLOCK_PIN 13  // Clock pin connected to shift register

// Digit select pins
#define NUM_DIGITS 3
uint8_t digitPins[NUM_DIGITS] = { 3, 4, 6 };

#define NUM_DISPLAYS 3
#define MAX_NUM_LENGTH 5  // Maximum length of input numbers (including decimal point)

// Timing
uint8_t delayTime = 5;  // Time in milliseconds for multiplexing

// Digit patterns for numbers 0-9
uint8_t digitPatterns[10] = {
  0b11000000,  // 0
  0b11111001,  // 1
  0b10100100,  // 2
  0b10110000,  // 3
  0b10011001,  // 4
  0b10010010,  // 5
  0b10000010,  // 6
  0b11111000,  // 7
  0b10000000,  // 8
  0b10010000   // 9
};

char nums[NUM_DISPLAYS][MAX_NUM_LENGTH] = { "1.23", "45.6", "789." };
char formattedNums[NUM_DISPLAYS][NUM_DIGITS + 1];  // +1 for null terminator
bool decimalFlags[NUM_DISPLAYS][NUM_DIGITS] = { { false } };

void setup() {
  // Initialize shift register pins
  pinMode(SHIFT_DATA_PIN, OUTPUT);
  pinMode(SHIFT_CLOCK_PIN, OUTPUT);
  pinMode(SHIFT_LATCH_PIN, OUTPUT);

  // Initialize digit select pins
  for (uint8_t i = 0; i < NUM_DIGITS; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);  // Ensure all digits are off at start
  }

  // Initial formatting of numbers for display
  formatForDisplay();

  // Update values dynamically
  updateDisplayValue(0, "-1");
  updateDisplayValue(1, "-12");
  updateDisplayValue(2, "1.23");
}

void loop() {
  static char lastNums[NUM_DISPLAYS][MAX_NUM_LENGTH] = { "" };
  bool needsUpdate = false;

  // Check if any numbers have changed
  for (uint8_t i = 0; i < NUM_DISPLAYS; i++) {
    if (strcmp(nums[i], lastNums[i]) != 0) {
      strcpy(lastNums[i], nums[i]);
      needsUpdate = true;
    }
  }

  // Reformat numbers if they've changed
  if (needsUpdate) {
    formatForDisplay();
  }

  // Continuously update the display
  displayFormattedNumbers();
}

// Function to format all numbers for display
void formatForDisplay() {
  for (uint8_t i = 0; i < NUM_DISPLAYS; i++) {
    formatToThreeChars(nums[i], formattedNums[i], decimalFlags[i]);
  }
}

// Function to format a single number into three characters and set decimal flags
void formatToThreeChars(const char input[], char output[], bool decimalFlags[]) {
  memset(output, ' ', NUM_DIGITS);
  memset(decimalFlags, false, NUM_DIGITS);

  uint8_t inputLength = strlen(input);
  if (inputLength > MAX_NUM_LENGTH - 1) {
    // Handle error: input too long (truncate if necessary)
    inputLength = MAX_NUM_LENGTH - 1;
  }

  int8_t outputIndex = NUM_DIGITS - 1;  // Start from the rightmost digit

  for (int8_t i = inputLength - 1; i >= 0; i--) {
    if (input[i] == '.') {
      if (outputIndex >= 0) {
        decimalFlags[outputIndex] = true;  // Set decimal point for the current digit
      }
    } else if ((input[i] >= '0' && input[i] <= '9') || input[i] == '-') {
      if (outputIndex >= 0) {
        output[outputIndex--] = input[i];  // Place digit or '-' in the output array
      }
    } else {
      // Handle unexpected characters (ignore or handle as needed)
    }
  }
  output[NUM_DIGITS] = '\0';  // Null-terminate the string
}

// Function to get the digit pattern, adding a decimal point if flagged
uint8_t getDigitPattern(char c, bool decimalPoint) {
  if (c >= '0' && c <= '9') {
    uint8_t pattern = digitPatterns[c - '0'];
    if (decimalPoint) {
      pattern &= ~(1 << 7);  // Clear the MSB to turn on the decimal point
    }
    return pattern;
  } else if (c == '-') {
    uint8_t pattern = 0b10111111;  // Pattern for '-'
    if (decimalPoint) {
      pattern &= ~(1 << 7);  // Clear the MSB to turn on the decimal point
    }
    return pattern;
  } else if (c == ' ') {
    return 0xFF;  // Blank display pattern
  } else {
    // Handle unexpected characters (return blank pattern)
    return 0xFF;
  }
}

// Function to select a digit to enable on the display
void selectDigit(uint8_t select) {
  for (uint8_t i = 0; i < NUM_DIGITS; i++) {
    digitalWrite(digitPins[i], (i == select) ? HIGH : LOW);
  }
}

// Function to display the formatted numbers
void displayFormattedNumbers() {
  static uint8_t currentDigit = 0;
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  // Control multiplexing timing without blocking
  if (currentTime - lastUpdateTime >= delayTime) {
    lastUpdateTime = currentTime;

    // Turn off all digits before updating
    for (uint8_t i = 0; i < NUM_DIGITS; i++) {
      digitalWrite(digitPins[i], LOW);
    }

    // Prepare shift registers for new data
    digitalWrite(SHIFT_LATCH_PIN, LOW);

    // Shift out the patterns for the current digit of each display
    for (int8_t display = NUM_DISPLAYS - 1; display >= 0; display--) {
      shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST,
               getDigitPattern(formattedNums[display][currentDigit], decimalFlags[display][currentDigit]));
    }

    // Latch data to outputs
    digitalWrite(SHIFT_LATCH_PIN, HIGH);

    // Activate the current digit
    digitalWrite(digitPins[currentDigit], HIGH);

    // Move to the next digit
    currentDigit = (currentDigit + 1) % NUM_DIGITS;
  }
}

void updateDisplayValue(uint8_t index, const char *newValue) {
  if (index >= NUM_DISPLAYS) {
    Serial.println("Error: Invalid index.");  // Debugging message
    return;                                   // Ensure index is within bounds
  }
  if (strlen(newValue) >= MAX_NUM_LENGTH) {
    Serial.println("Error: Value too long.");  // Debugging message
    return;                                    // Ensure newValue fits within MAX_NUM_LENGTH
  }

  strncpy(nums[index], newValue, MAX_NUM_LENGTH - 1);
  nums[index][MAX_NUM_LENGTH - 1] = '\0';  // Ensure null termination
}
