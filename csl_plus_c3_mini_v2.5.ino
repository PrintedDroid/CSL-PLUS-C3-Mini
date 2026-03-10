/*
================================================================================
// PROJECT:       Interactive LED Animations for CSL+ and LDPL
// AUTHOR:        Printed-Droid.com, Michael Whitley (Original)
// DATE:          September 15, 2025
// VERSION:       2.5 ESP32 C3 Mini Edition with FastLED Compatibility Fix

// DESCRIPTION:
// This sketch is a comprehensive control suite for the "Coin Slot" (CSL+)
// and "Large Droid Panel Logic" (LDPL) LED panels from Printed-Droid.com.
// It is specifically designed for the ESP32 C3 Mini and provides interactive
// control via six push buttons and the Arduino Serial Monitor.
// The feature set includes a variety of animations, a fully functional "Simon" 
// memory game, and the ability to permanently save settings.
// This version adds a hardware kill-switch input to immediately turn off all LEDs.


// VERSION HISTORY:
// - v1.0 (Original by M. Whitley):
//     - Basic set of animations (e.g., sinelon, beatit, rainbow).
//     - Simple control scheme: One button per animation, long press to
//       toggle color mode.
//     - Ported to various boards (Arduino 328p, ESP32).
//
// - v2.0 (ESP32 C3 Mini Edition):
//     - Complete refactor of the code architecture for the ESP32 C3 Mini.
//     - Added: Control for a second LED panel (LDPL).
//     - New control concept: Parameterized control for power state,
//       animation, speed, color, and brightness.
//     - Added: Persistent storage of settings via Preferences.h.
//     - Added: Fully functional "Simon" memory game.
//     - Animation set revised; some older effects were replaced with new ones
//       (fire, comet, alarm).
//
// - v2.1 (Serial Control Edition):
//     - Added: Full control via the Arduino IDE Serial Monitor.
//     - New function `handleSerialCommands()` to parse and execute text commands.
//     - Added "help" command to list all available serial commands.
//     - Main documentation updated with serial control instructions.
//
// - v2.2 (Kill-Switch Edition):
//     - Added: A hardware kill-switch on a dedicated pin (IO8).
//     - When the kill-switch pin is grounded, all LEDs turn off and all other
//       logic is bypassed until the switch is opened.
//
// - v2.3 (Enhanced CLI Edition):
//     - Added: 'status' command to show current system status.
//     - Added: 'config' command to display all current settings.
//     - Added: 'reset' command to restart the system.
//     - Added: Serial feedback for all button actions.
//     - Added: Bounds validation for EEPROM-loaded values.
//     - Fixed: Demo mode now respects manual animation selection.
//     - Fixed: Explicit type casts for beatsin functions.
//     - Improved: Code documentation and comments.
//
// - v2.4 (RGB Color Edition):
//     - Added: Custom RGB color support via 'rgb=R,G,B' command.
//     - Color mode 17 = custom RGB (in addition to 0-16 palette modes).
//     - RGB values are saved/loaded with other settings.
//     - Config output shows RGB values when in custom mode.
//
// - v2.5 (FastLED Compatibility Fix + Bugfixes):
//     - IMPORTANT: Requires FastLED version 3.9.4 or earlier!
//     - FastLED 3.9.5+ has a known ESP32-C3 RMT driver bug with multiple strips.
//     - See: https://github.com/FastLED/FastLED/issues/1498
//     - Fixed: Array out-of-bounds crash when colorMode=17 in largelogic().
//     - Fixed: Button color cycling now includes custom RGB (mode 17).
//     - Fixed: fire() animation double-random bug (different indices).
//     - Added: Max power limit (2A) to prevent brownouts/resets.
//     - Improved: Simon game now uses fixed colors per segment (better UX).

// HARDWARE REQUIREMENTS:
// - Board: ESP32 C3 Mini (e.g., Lolin C3 Mini)
// - LEDs: WS2812 / "NeoPixel" type LED strips
//     -> CSL Panel: 36 LEDs
//     -> LDPL Panel: 43 LEDs
// - 6x Push Buttons for control.
// - 1x External Switch for the kill-switch feature.

// PINOUT (ESP32 C3 Mini):
// - IO0: Button 1 (Power)
// - IO1: Button 2 (Animation)
// - IO2: Button 3 (Speed)
// - IO3: Button 4 (Color)
// - IO4: Button 5 (Brightness)
// - IO5: Button 6 (Mode / Save)
// - IO6: Data Line for CSL LED Panel
// - IO7: Data Line for LDPL LED Panel
// - IO8: Kill-Switch Input (Connect to GND to turn off LEDs)

// REQUIRED LIBRARIES:
// Install via the Arduino Library Manager:
// - FastLED (by Daniel Garcia) - *** VERSION 3.9.4 REQUIRED! ***
//   (FastLED 3.9.5+ has RMT driver bugs on ESP32-C3 with multiple LED strips)
// - Preferences.h (is part of the ESP32 core package, no extra installation needed)

// BUTTON CONTROLS:
// - Button 1: Cycle Power State (On -> Demo -> Off).
// - Button 2: Select next animation.
// - Button 3: Change animation speed (4 levels).
// - Button 4: Select color (including "multi-color").
// - Button 5: Change brightness (5 levels).
// - Button 6 (Short Press): Toggle Mode (Animation <-> Simon Game).
// - Button 6 (Long Press): Save current settings permanently.

// ================================================================================
// SERIAL MONITOR CONTROLS:
// 1. Open the Serial Monitor in the Arduino IDE.
// 2. Set the baud rate to 9600.
// 3. Set the line ending to "Newline".
// 4. Type 'help' and press Enter to see all available commands.
//
// COMMAND STRUCTURE: command=value (e.g., anim=3) or just command (e.g., save)
//
// AVAILABLE COMMANDS:
// - help:           Displays this list of commands.
// - status:         Shows current system status (power, mode, uptime).
// - config:         Shows all current configuration values.
// - anim=<0-6>:     Selects an animation.
// - speed=<0-3>:    Sets the animation speed index.
// - bright=<0-4>:   Sets the brightness level index.
// - color=<0-17>:   Sets the color mode (0=multi, 1-16=palette, 17=custom RGB).
// - rgb=R,G,B:      Sets custom RGB color (0-255 each), activates mode 17.
// - power=<0-2>:    Sets the power state (0=ON, 1=OFF, 2=DEMO).
// - mode=<0-1>:     Sets the main mode (0=Animation, 1=Simon Game).
// - save:           Saves the current settings to permanent memory.
// - reset:          Restarts the system.

// WIRING THE KILL-SWITCH:
// Connect a switch between Pin IO8 and a GND pin on the ESP32.
// When the switch is closed, the circuit is completed, and the LEDs will turn off.
// ================================================================================
*/

// =============================================================================
// FASTLED LIBRARY VERSION REQUIREMENT
// =============================================================================
// *** IMPORTANT: Use FastLED version 3.9.4 or earlier! ***
//
// FastLED 3.9.5+ has a known bug on ESP32-C3 with multiple LED strips:
// - ESP32-C3 has only 2 RMT TX channels
// - FastLED 3.9.5+ RMT driver doesn't handle this properly
// - Results in "no free tx channels" error and reboot loop
//
// The bug is documented at: https://github.com/FastLED/FastLED/issues/1498
//
// To install correct version:
// Arduino IDE: Sketch -> Include Library -> Manage Libraries -> FastLED -> 3.9.4
// =============================================================================

#include <FastLED.h>
#include <Preferences.h>

// --- Pin Definitions ---
#define BUTTON1_PIN 0
#define BUTTON2_PIN 1
#define BUTTON3_PIN 2
#define BUTTON4_PIN 3
#define BUTTON5_PIN 4
#define BUTTON6_PIN 5
#define NUM_BUTTONS 6
#define KILL_SWITCH_PIN 8 // Pin for the hardware kill-switch

// --- LED Strip Definitions ---
#define CSL_PIN     6
#define NUMLEDS_CSL 36
CRGB leds_csl[NUMLEDS_CSL];
#define LDPL_PIN    7
#define NUMLEDS_LDPL 43
CRGB leds_ldpl[NUMLEDS_LDPL];

// --- General Configuration ---
#define NUM_ANIMATIONS 7

// --- Preferences Object for Saving Settings ---
Preferences preferences;

// --- State & Mode Control Variables ---
enum PowerState { STATE_ON, STATE_OFF, STATE_DEMO };
PowerState powerState = STATE_DEMO;
int mainMode = 0;
int animationMode = 0;
bool demoOverride = false; // True when user manually selected animation in demo mode

// --- Unified Button Handling System ---
byte prevButtonStates[NUM_BUTTONS];
unsigned long buttonPressTime[NUM_BUTTONS];
// Long press threshold in sample counts (not milliseconds!)
// Actual duration = longPressThreshold * sampleInterval (25ms) = 80 * 25 = 2000ms
const unsigned long longPressThreshold = 80;

// --- Parameter Control ---
float speedLevels[] = {0.5, 1.0, 1.5, 2.0};
uint8_t speedIndex = 1;
uint8_t brightnessLevels[] = {10, 40, 90, 160, 255};
uint8_t brightnessIndex = 2;
uint8_t colorPalette[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240};
#define NUM_COLORS (sizeof(colorPalette) / sizeof(colorPalette[0]))
int colorMode = 0;  // 0=multi, 1-16=palette, 17=custom RGB
CRGB primaryColor;

// --- Custom RGB Color (colorMode = 17) ---
uint8_t customR = 255;
uint8_t customG = 0;
uint8_t customB = 0;

// --- Simon Game Configuration ---
enum GameState { SIMON_OFF, SIMON_START, SIMON_SHOW_SEQUENCE, SIMON_WAIT_FOR_INPUT, SIMON_LOSE, SIMON_WIN };
GameState simonState = SIMON_OFF;
int simonSequence[20];
int simonCurrentLevel = 0;
int simonPlayerStep = 0;

// --- Animation Variables ---
int demoIndex = 0;
uint8_t gHue = 0;
int buttonList[] = {BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN, BUTTON4_PIN, BUTTON5_PIN, BUTTON6_PIN};
byte heat[NUMLEDS_CSL];
uint8_t lHue = 0;
int llAnimate = 0;
const int llSweepPeriod = 20;

// --- Forward Declarations ---
void beatit(bool, float); void breathe(bool, float);
void juggle(bool, float);
void sparkle(bool, float); void comet(bool, float); void alarm(bool, float);
void fire(bool, float); void largelogic(bool, float);

// ================================================================
// SETUP
// ================================================================
void setup() {
  // Initialize standard buttons
  for(int i=0; i<NUM_BUTTONS; i++) {
    pinMode(buttonList[i], INPUT_PULLUP);
    prevButtonStates[i] = HIGH;
  }
  
  // NEW: Initialize the kill-switch pin
  // INPUT_PULLUP means the pin reads HIGH when the switch is open,
  // and LOW when the switch is closed (connected to GND).
  pinMode(KILL_SWITCH_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(2000);
  Serial.println("CSL+ v2.5 Initialized. Type 'help' for commands.");

  loadSettings();

  // Initialize LED strips
  FastLED.addLeds<WS2812, CSL_PIN, GRB>(leds_csl, NUMLEDS_CSL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, LDPL_PIN, GRB>(leds_ldpl, NUMLEDS_LDPL).setCorrection(TypicalLEDStrip);

  // Limit max power to prevent brownouts/resets on ESP32-C3
  // 79 LEDs * 60mA = 4.74A max theoretical, limit to safe value
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000); // 2A max

  updatePrimaryColor();
  FastLED.setBrightness(brightnessLevels[brightnessIndex]);

  fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Blue);
  fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Blue);
  FastLED.show();
  delay(500);
}

// ================================================================
// MAIN LOOP
// ================================================================
void loop() {
  // NEW: Check the kill-switch at the very beginning of the loop.
  // If the switch is closed (pin is LOW), turn off all LEDs and skip all other logic.
  if (digitalRead(KILL_SWITCH_PIN) == LOW) {
    FastLED.clear(true); // Clears all LED data and pushes the update to the strips.
    delay(50);           // A small delay to prevent the loop from running too fast.
    return;              // Immediately exits the loop function for this cycle.
  }
  
  // If the kill-switch is not active, run the normal program logic.
  handleButtons();
  handleSerialCommands(); 
  
  if (mainMode == 0) {
    animate();
  } else {
    runSimonGame();
  }
}

// ================================================================
// SETTINGS & BUTTONS
// ================================================================
void loadSettings() {
  preferences.begin("led-settings", true);
  powerState = (PowerState)preferences.getUChar("powerState", STATE_DEMO);
  animationMode = preferences.getUChar("animMode", 0);
  speedIndex = preferences.getUChar("speedIdx", 1);
  colorMode = preferences.getUChar("colorMode", 0);
  brightnessIndex = preferences.getUChar("brightIdx", 2);
  customR = preferences.getUChar("customR", 255);
  customG = preferences.getUChar("customG", 0);
  customB = preferences.getUChar("customB", 0);
  preferences.end();

  // Validate loaded values to prevent corruption issues
  if (powerState > STATE_DEMO) powerState = STATE_DEMO;
  if (animationMode >= NUM_ANIMATIONS) animationMode = 0;
  if (speedIndex >= 4) speedIndex = 1;
  if (colorMode > 17) colorMode = 0;  // Now supports 0-17 (17=custom RGB)
  if (brightnessIndex >= 5) brightnessIndex = 2;

  updatePrimaryColor();  // Apply loaded color
  Serial.println("Settings loaded from EEPROM.");
}

void saveSettings() {
  preferences.begin("led-settings", false);
  preferences.putUChar("powerState", (uint8_t)powerState);
  preferences.putUChar("animMode", animationMode);
  preferences.putUChar("speedIdx", speedIndex);
  preferences.putUChar("colorMode", colorMode);
  preferences.putUChar("brightIdx", brightnessIndex);
  preferences.putUChar("customR", customR);
  preferences.putUChar("customG", customG);
  preferences.putUChar("customB", customB);
  preferences.end();
  
  for(int i=0; i<2; i++) {
    fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Green);
    fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Green);
    FastLED.show(); delay(80);
    fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
    fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Black);
    FastLED.show(); delay(80);
  }
}

void updatePrimaryColor() {
  if (colorMode == 17) {
    // Custom RGB mode
    primaryColor = CRGB(customR, customG, customB);
  } else if (colorMode > 0 && colorMode <= NUM_COLORS) {
    // Palette mode (1-16)
    primaryColor = CHSV(colorPalette[colorMode - 1], 255, 255);
  }
  // colorMode 0 = multi-color, handled in animations
}

// Helper function to get power state name
const char* getPowerStateName(PowerState state) {
  switch(state) {
    case STATE_ON: return "ON";
    case STATE_OFF: return "OFF";
    case STATE_DEMO: return "DEMO";
    default: return "UNKNOWN";
  }
}

// Helper arrays for animation and mode names
const char* animationNames[] = {"beatit", "breathe", "juggle", "sparkle", "comet", "alarm", "fire"};

void handleButtons() {
  static unsigned long lastSampleTime = 0;
  if (millis() - lastSampleTime < 25) { return; }
  lastSampleTime = millis();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    byte currentState = digitalRead(buttonList[i]);
    if (currentState == LOW && prevButtonStates[i] == HIGH) {
      buttonPressTime[i] = millis();
    } else if (currentState == HIGH && prevButtonStates[i] == LOW) {
      unsigned long pressDuration = (millis() - buttonPressTime[i]) / 25;
      switch(i) {
        case 0:
          powerState = (PowerState)((powerState + 1) % 3);
          if (powerState != STATE_DEMO) demoOverride = false;
          Serial.print("Button: Power -> ");
          Serial.println(getPowerStateName(powerState));
          break;
        case 1:
          animationMode = (animationMode + 1) % NUM_ANIMATIONS;
          if (powerState == STATE_DEMO) demoOverride = true;
          Serial.print("Button: Animation -> ");
          Serial.print(animationMode);
          Serial.print(" (");
          Serial.print(animationNames[animationMode]);
          Serial.println(")");
          break;
        case 2:
          speedIndex = (speedIndex + 1) % 4;
          Serial.print("Button: Speed -> ");
          Serial.print(speedIndex);
          Serial.print(" (");
          Serial.print(speedLevels[speedIndex]);
          Serial.println("x)");
          break;
        case 3:
          colorMode = (colorMode + 1) % (NUM_COLORS + 2); // 0-17 (includes custom RGB)
          updatePrimaryColor();
          Serial.print("Button: Color -> ");
          if (colorMode == 0) Serial.println("Multi-color");
          else if (colorMode == 17) Serial.println("Custom RGB");
          else Serial.println(colorMode);
          break;
        case 4:
          brightnessIndex = (brightnessIndex + 1) % 5;
          FastLED.setBrightness(brightnessLevels[brightnessIndex]);
          Serial.print("Button: Brightness -> ");
          Serial.print(brightnessIndex);
          Serial.print(" (");
          Serial.print(brightnessLevels[brightnessIndex]);
          Serial.println(")");
          break;
        case 5:
          if (pressDuration >= longPressThreshold) {
            saveSettings();
            Serial.println("Button: Settings saved!");
          } else {
            mainMode = (mainMode == 0) ? 1 : 0;
            simonState = (mainMode == 1) ? SIMON_START : SIMON_OFF;
            Serial.print("Button: Mode -> ");
            Serial.println(mainMode == 0 ? "Animation" : "Simon Game");
          }
          break;
      }
    }
    prevButtonStates[i] = currentState;
  }
}

// ================================================================
// SERIAL COMMAND HANDLER
// ================================================================
void handleSerialCommands() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); 

    String command;
    int value;
    int separatorIndex = input.indexOf('=');

    if (separatorIndex != -1) {
      command = input.substring(0, separatorIndex);
      value = input.substring(separatorIndex + 1).toInt();
    } else {
      command = input;
      value = -1; 
    }

    if (command == "help") {
      Serial.println("--- Command List ---");
      Serial.println("status           - Show current system status");
      Serial.println("config           - Show all current settings");
      Serial.println("anim=<0-6>       - Select animation");
      Serial.println("speed=<0-3>      - Set speed index");
      Serial.println("bright=<0-4>     - Set brightness index");
      Serial.println("color=<0-17>     - Set color mode (0=multi, 1-16=palette, 17=RGB)");
      Serial.println("rgb=R,G,B        - Set custom RGB color (0-255 each)");
      Serial.println("power=<0-2>      - Set power state (0=ON, 1=OFF, 2=DEMO)");
      Serial.println("mode=<0-1>       - Set main mode (0=Animation, 1=Simon Game)");
      Serial.println("save             - Save current settings");
      Serial.println("reset            - Restart the system");
      Serial.println("--------------------");
    } else if (command == "status") {
      Serial.println("--- System Status ---");
      Serial.print("Power: ");
      Serial.println(getPowerStateName(powerState));
      Serial.print("Mode: ");
      Serial.println(mainMode == 0 ? "Animation" : "Simon Game");
      if (mainMode == 0) {
        Serial.print("Animation: ");
        Serial.print(animationMode);
        Serial.print(" (");
        Serial.print(animationNames[animationMode]);
        Serial.println(")");
        Serial.print("Demo Override: ");
        Serial.println(demoOverride ? "Yes" : "No");
      }
      Serial.print("Uptime: ");
      Serial.print(millis() / 1000);
      Serial.println(" seconds");
      Serial.println("--------------------");
    } else if (command == "config") {
      Serial.println("--- Current Configuration ---");
      Serial.print("Power State: ");
      Serial.print((int)powerState);
      Serial.print(" (");
      Serial.print(getPowerStateName(powerState));
      Serial.println(")");
      Serial.print("Animation: ");
      Serial.print(animationMode);
      Serial.print(" (");
      Serial.print(animationNames[animationMode]);
      Serial.println(")");
      Serial.print("Speed: ");
      Serial.print(speedIndex);
      Serial.print(" (");
      Serial.print(speedLevels[speedIndex]);
      Serial.println("x)");
      Serial.print("Brightness: ");
      Serial.print(brightnessIndex);
      Serial.print(" (");
      Serial.print(brightnessLevels[brightnessIndex]);
      Serial.println(")");
      Serial.print("Color Mode: ");
      if (colorMode == 0) {
        Serial.println("0 (Multi-color)");
      } else if (colorMode == 17) {
        Serial.print("17 (RGB: ");
        Serial.print(customR); Serial.print(",");
        Serial.print(customG); Serial.print(",");
        Serial.print(customB); Serial.println(")");
      } else {
        Serial.print(colorMode);
        Serial.println(" (Palette)");
      }
      Serial.print("Main Mode: ");
      Serial.println(mainMode == 0 ? "0 (Animation)" : "1 (Simon Game)");
      Serial.println("--------------------");
    } else if (command == "reset") {
      Serial.println("Restarting system...");
      delay(500);
      ESP.restart();
    } else if (command == "anim") {
      if (value >= 0 && value < NUM_ANIMATIONS) {
        animationMode = value;
        if (powerState == STATE_DEMO) demoOverride = true; // User manually selected in demo mode
        Serial.println("OK: Animation set to " + String(value));
      } else {
        Serial.println("Error: Invalid value for 'anim'. Range is 0-" + String(NUM_ANIMATIONS - 1));
      }
    } else if (command == "speed") {
      if (value >= 0 && value < 4) {
        speedIndex = value;
        Serial.println("OK: Speed index set to " + String(value));
      } else {
        Serial.println("Error: Invalid value for 'speed'. Range is 0-3.");
      }
    } else if (command == "bright") {
      if (value >= 0 && value < 5) {
        brightnessIndex = value;
        FastLED.setBrightness(brightnessLevels[brightnessIndex]);
        Serial.println("OK: Brightness index set to " + String(value));
      } else {
        Serial.println("Error: Invalid value for 'bright'. Range is 0-4.");
      }
    } else if (command == "color") {
      if (value >= 0 && value <= 17) {  // 0-17 (17=custom RGB)
        colorMode = value;
        updatePrimaryColor();
        Serial.println("OK: Color mode set to " + String(value));
      } else {
        Serial.println("Error: Invalid value for 'color'. Range is 0-17.");
      }
    } else if (command == "power") {
        if (value >= 0 && value <= 2) {
            powerState = (PowerState)value;
            if (powerState != STATE_DEMO) demoOverride = false; // Reset override when leaving demo
            Serial.println("OK: Power state set to " + String(value));
        } else {
            Serial.println("Error: Invalid value for 'power'. Range is 0-2.");
        }
    } else if (command == "mode") {
        if (value >= 0 && value <= 1) {
            mainMode = value;
            simonState = (mainMode == 1) ? SIMON_START : SIMON_OFF;
            Serial.println("OK: Main mode set to " + String(value));
        } else {
            Serial.println("Error: Invalid value for 'mode'. Range is 0-1.");
        }
    } else if (command == "rgb") {
      // Parse RGB format: rgb=R,G,B
      String rgbValue = input.substring(separatorIndex + 1);
      int comma1 = rgbValue.indexOf(',');
      int comma2 = rgbValue.lastIndexOf(',');
      if (comma1 > 0 && comma2 > comma1) {
        int r = rgbValue.substring(0, comma1).toInt();
        int g = rgbValue.substring(comma1 + 1, comma2).toInt();
        int b = rgbValue.substring(comma2 + 1).toInt();
        if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
          customR = r;
          customG = g;
          customB = b;
          colorMode = 17;  // Switch to custom RGB mode
          updatePrimaryColor();
          Serial.print("OK: RGB set to ");
          Serial.print(r); Serial.print(",");
          Serial.print(g); Serial.print(",");
          Serial.println(b);
        } else {
          Serial.println("Error: RGB values must be 0-255.");
        }
      } else {
        Serial.println("Error: Use format rgb=R,G,B (e.g., rgb=255,0,128)");
      }
    } else if (command == "save") {
      saveSettings();
      Serial.println("OK: Settings saved.");
    } else {
      Serial.println("Unknown command. Type 'help' for a list of commands.");
    }
  }
}

// ================================================================
// ANIMATION ENGINE
// ================================================================
void animate() {
  bool isSingleColor = (colorMode > 0);
  float currentSpeed = speedLevels[speedIndex];
  
  if (powerState == STATE_OFF) {
    fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
    fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Black);
    FastLED.show();
    return;
  }
  
  if (powerState == STATE_DEMO && !demoOverride) {
    EVERY_N_SECONDS(10) { demoIndex = (demoIndex + 1) % NUM_ANIMATIONS; }
    animationMode = demoIndex;
  }

  // Run animation for the main CSL panel
  switch (animationMode) {
    case 0: beatit(isSingleColor, currentSpeed); break;
    case 1: breathe(isSingleColor, currentSpeed); break;
    case 2: juggle(isSingleColor, currentSpeed); break;
    case 3: sparkle(isSingleColor, currentSpeed); break;
    case 4: comet(isSingleColor, currentSpeed); break;
    case 5: alarm(isSingleColor, currentSpeed); break;
    case 6: fire(isSingleColor, currentSpeed); break;
  }

  // For non-synced animations, run the default LDPL effect
  if (animationMode != 5 && animationMode != 6) {
    largelogic(isSingleColor, currentSpeed);
  }

  FastLED.show();
  EVERY_N_MILLISECONDS(20) { gHue++; lHue++; }
}

void restoreBrightness() {
  FastLED.setBrightness(brightnessLevels[brightnessIndex]);
}

// ================================================================
// ANIMATION FUNCTIONS
// ================================================================

void largelogic(bool useSingleColor, float speed) {
    restoreBrightness();
    uint8_t beat = beatsin8((uint8_t)(5 * speed), 30, 255);
    EVERY_N_SECONDS(llSweepPeriod) { llAnimate = 1; lHue = 0;}

    uint8_t currentHue;
    if (useSingleColor) {
        if (colorMode == 17) {
            // Custom RGB: derive hue from primaryColor
            CHSV hsv = rgb2hsv_approximate(primaryColor);
            currentHue = hsv.h;
        } else if (colorMode > 0 && colorMode <= NUM_COLORS) {
            currentHue = colorPalette[colorMode - 1];
        } else {
            currentHue = 170; // fallback
        }
    } else {
        currentHue = 170;
    }
    
    fill_solid(leds_ldpl, NUMLEDS_LDPL, CHSV(currentHue, 255, beat));

    if (llAnimate == 1) {
      for(int i = 0; i < NUMLEDS_LDPL; i++) {
        int distance = abs(i - (int)lHue);
        if (distance < 10) {
          int brightness = 255 - (distance * 25);
          leds_ldpl[i] = CHSV(currentHue, 255, max(beat, (uint8_t)brightness));
        }
      }
      if (lHue > NUMLEDS_LDPL + 10) { llAnimate = 0; }
    }
}

void alarm(bool useSingleColor, float speed) {
  uint8_t flash = beatsin8((uint8_t)(180 * speed), 0, 2);
  CRGB color = useSingleColor ? primaryColor : CRGB::Red;
  if (flash > 0) {
    fill_solid(leds_csl, NUMLEDS_CSL, color);
    fill_solid(leds_ldpl, NUMLEDS_LDPL, color);
  } else {
    fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
    fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Black);
  }
}

void fire(bool useSingleColor, float speed) {
  for( int i = 0; i < NUMLEDS_CSL; i++) {
    heat[i] = qsub8( heat[i], random8(0, ((55 * 10) / NUMLEDS_CSL) + 2));
  }
  for( int k = NUMLEDS_CSL - 1; k >= 2; k--) {
    heat[k] = (heat[k-1] + heat[k-2] + heat[k-2]) / 3;
  }
  if( random8() < 120 * speed ) {
    uint8_t y = random8(NUMLEDS_CSL);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  for( int j = 0; j < NUMLEDS_CSL; j++) { leds_csl[j] = HeatColor(heat[j]); }
  for( int j = 0; j < NUMLEDS_LDPL; j++) { leds_ldpl[j] = HeatColor(heat[j % NUMLEDS_CSL]); }
}

void sparkle(bool useSingleColor, float speed) {
  fadeToBlackBy(leds_csl, NUMLEDS_CSL, 20);
  if(random8() < (120 * speed)) {
    leds_csl[random16(NUMLEDS_CSL)] += useSingleColor ? primaryColor : CHSV(random8(), 200, 255);
  }
}

void breathe(bool useSingleColor, float speed) {
  restoreBrightness();
  float breath = (exp(sin(millis() / (5000.0 / speed) * PI)) - 0.36787944) * 108.0;
  FastLED.setBrightness(map(breath, 0, 255, 10, brightnessLevels[brightnessIndex]));
  fill_solid(leds_csl, NUMLEDS_CSL, useSingleColor ? primaryColor : CHSV(gHue, 255, 255));
}

void juggle(bool useSingleColor, float speed) {
  fadeToBlackBy(leds_csl, NUMLEDS_CSL, 20);
  for (int i = 0; i < 7; i++) {
    leds_csl[beatsin16((uint16_t)((i + 7) * speed), 0, NUMLEDS_CSL - 1)] |= useSingleColor ?
      primaryColor : CHSV(i * 32, 200, 255);
  }
}

void comet(bool useSingleColor, float speed) {
  fadeToBlackBy(leds_csl, NUMLEDS_CSL, 40);
  leds_csl[beatsin16((uint16_t)(20 * speed), 0, NUMLEDS_CSL-1)] = useSingleColor ? primaryColor : CHSV(gHue, 255, 255);
}

void beatit(bool useSingleColor, float speed) {
  fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
  uint8_t beat = beatsin8((uint8_t)(60 * speed), 0, 7);
  for (int i = 0; i < 6; i++) {
    if (beat > i) {
      for (int j = 0; j < 6; j++) {
        leds_csl[(5 - i) * 6 + j] = useSingleColor ?
          primaryColor : CHSV(gHue, 255, 192);
      }
    }
  }
}

// ================================================================
// SIMON GAME
// ================================================================
void runSimonGame() {
  restoreBrightness();
  fill_solid(leds_ldpl, NUMLEDS_LDPL, CRGB::Black);

  switch (simonState) {
    case SIMON_START:
      playSimonAnimation(CRGB::Blue, 2);
      simonCurrentLevel = 1;
      randomSeed(millis());
      simonSequence[0] = random(6);
      simonState = SIMON_SHOW_SEQUENCE;
      break;
    case SIMON_SHOW_SEQUENCE:
      fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black); FastLED.show(); delay(500);
      for (int i = 0; i < simonCurrentLevel; i++) {
        showSimonSegment(simonSequence[i], 400);
      }
      simonPlayerStep = 0;
      simonState = SIMON_WAIT_FOR_INPUT;
      break;
    case SIMON_WAIT_FOR_INPUT:
      {
        int button = getButtonPressed();
        if (button != -1) {
          if (button == simonSequence[simonPlayerStep]) {
            simonPlayerStep++;
            if (simonPlayerStep >= simonCurrentLevel) {
              if (simonCurrentLevel >= 20) { simonState = SIMON_WIN; } 
              else { simonState = SIMON_SHOW_SEQUENCE; simonCurrentLevel++; simonSequence[simonCurrentLevel - 1] = random(6); delay(500); }
            }
          } else { simonState = SIMON_LOSE; }
        }
      }
      break;
    case SIMON_LOSE: playSimonAnimation(CRGB::Red, 3); simonState = SIMON_START; break;
    case SIMON_WIN: playSimonAnimation(CRGB::Green, 5); simonState = SIMON_START; break;
    case SIMON_OFF: break; // Not active, do nothing
  }
}

// Fixed colors for Simon segments (better recognition than random)
const CRGB simonColors[6] = {
  CRGB::Red, CRGB::Green, CRGB::Blue,
  CRGB::Yellow, CRGB::Magenta, CRGB::Cyan
};

void showSimonSegment(int index, int duration) {
  CRGB color = simonColors[index % 6];
  int startLed = index * 6;
  for (int i = 0; i < 6; i++) { leds_csl[startLed + i] = color; }
  FastLED.show(); delay(duration);
  fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
  FastLED.show(); delay(50);
}

void playSimonAnimation(CRGB color, int count) {
  for(int i = 0; i < count; i++) {
    fill_solid(leds_csl, NUMLEDS_CSL, color);
    FastLED.show(); delay(150);
    fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black); FastLED.show(); delay(150);
  }
}

int getButtonPressed() {
  for (int i = 0; i < 6; i++) {
    if (digitalRead(buttonList[i]) == LOW) {
      unsigned long pressStart = millis();
      int startLed = i * 6;
      for(int j=0; j<6; j++) { leds_csl[startLed+j] = CRGB::White; }
      FastLED.show();

      // Wait for button release, check for long press on Button 6
      while(digitalRead(buttonList[i]) == LOW) {
        // Button 6 long press (>1s) exits Simon Game
        if (i == 5 && (millis() - pressStart) > 1000) {
          fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
          FastLED.show();
          mainMode = 0;
          simonState = SIMON_OFF;
          Serial.println("Simon Game: Exited via long press");
          return -1;
        }
        delay(10);
      }
      fill_solid(leds_csl, NUMLEDS_CSL, CRGB::Black);
      FastLED.show();
      return i;
    }
  }
  return -1;
}
