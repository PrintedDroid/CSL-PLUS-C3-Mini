# CSL+ C3 Mini LED Controller v2.5
**Interactive LED Animation Controller for CSL+ and LDPL Panels on ESP32 C3 Mini**

## Project Overview

This comprehensive control suite brings your "Coin Slot" (CSL+) and "Large Droid Panel Logic" (LDPL) LED panels to life with interactive animations, a built-in Simon memory game, and flexible control options.
Designed specifically for the ESP32 C3 Mini with professional-grade features for R2 builders.

### Key Features

- **Dual LED Panel Control** - CSL (36 LEDs) + LDPL (43 LEDs) with synchronized animations
- **7 Animation Modes** - beatit, breathe, juggle, sparkle, comet, alarm, fire
- **Hardware Kill-Switch** - Dedicated IO8 input for immediate LED shutoff
- **6-Button Control** - Direct hardware control for all parameters
- **Enhanced Serial CLI** - Full configuration and monitoring via text commands
- **Custom RGB Colors** - Set any color via `rgb=R,G,B` command
- **Simon Memory Game** - Built-in interactive game using the LED panels
- **Demo Mode** - Automatic animation cycling for displays
- **Persistent Settings** - EEPROM storage for all configurations
- **18 Color Modes** - Multi-color + 16 palette + custom RGB

---

## Changelog

### Version 2.5 (January 2026)

**Bugfixes + FastLED Version Requirement**

- **⚠️ IMPORTANT**: Requires **FastLED version 3.9.4**!
- FastLED 3.9.5+ has a known RMT driver bug on ESP32-C3 with multiple LED strips
- See: [FastLED Issue #1498](https://github.com/FastLED/FastLED/issues/1498)
- **FIXED**: Array out-of-bounds crash when using custom RGB (colorMode=17)
- **FIXED**: Button color cycling now includes custom RGB (mode 17)
- **FIXED**: fire() animation double-random bug
- **ADDED**: Max power limit (2A) to prevent brownouts/resets
- **IMPROVED**: Simon game uses fixed colors per segment (better recognition)

### Version 2.4 (November 2025)

**RGB Color Edition**

- **NEW**: Custom RGB color support via `rgb=R,G,B` command
- **NEW**: Color mode 17 = custom RGB (any color you want!)
- **NEW**: RGB values are saved/loaded with other settings
- Supports both palette colors (1-16) AND custom RGB (17)
- Use `config` to see current RGB values

### Version 2.3 (November 2025)

**Enhanced CLI Edition**

- **NEW**: `status` command to show current system status
- **NEW**: `config` command to display all configuration values
- **NEW**: `reset` command to restart the system
- **NEW**: Serial feedback for all button actions
- **FIXED**: Demo mode now respects manual animation selection
- **FIXED**: Explicit type casts for beatsin functions (cleaner code)
- **IMPROVED**: Bounds validation for EEPROM-loaded values
- **IMPROVED**: Code documentation and comments

### Version 2.2 (September 2025)

**Kill-Switch Edition**

- **NEW**: Hardware kill-switch on dedicated pin (IO8)
- When kill-switch pin is grounded, all LEDs turn off instantly
- All logic bypassed until switch is opened
- Ideal for safety shutoff during events or transport

### Version 2.1

**Serial Control Edition**

- **NEW**: Full control via Arduino IDE Serial Monitor
- New `handleSerialCommands()` function for text command parsing
- Added `help` command to display all available commands
- Complete serial control documentation

### Version 2.0

**ESP32 C3 Mini Edition**

- Complete code architecture refactor for ESP32 C3 Mini
- Added control for second LED panel (LDPL)
- New parameterized control concept (power, animation, speed, color, brightness)
- Persistent settings storage via Preferences.h
- Built-in "Simon" memory game
- New animations: fire, comet, alarm (replacing older effects)

### Version 1.0 (Original by M. Whitley)

- Basic animation set (sinelon, beatit, rainbow)
- Simple control: one button per animation
- Long press for color mode toggle
- Ported to various boards (Arduino 328p, ESP32)

---

## Hardware Requirements

### Core Components
- **ESP32 C3 Mini** (e.g., Lolin C3 Mini) - recommended controller
- **WS2812 / NeoPixel LED Strips**:
  - CSL Panel: 36 LEDs
  - LDPL Panel: 43 LEDs
- **6x Push Buttons** - for direct control
- **1x External Switch** - for kill-switch feature
- **5V Power Supply** - adequate for LED strips (minimum 3A recommended)

### Optional Components
- Enclosure for electronics
- Panel diffusers for LED effects

## Pin Configuration

ESP32 C3 Mini Pin Assignments:

```
IO0 → Button 1 (Power State)
IO1 → Button 2 (Animation)
IO2 → Button 3 (Speed)
IO3 → Button 4 (Color)
IO4 → Button 5 (Brightness)
IO5 → Button 6 (Short: Simon Game / Long: Save)
IO6 → Data Line for CSL LED Panel (36 LEDs)
IO7 → Data Line for LDPL LED Panel (43 LEDs)
IO8 → Kill-Switch Input (Connect to GND to turn off LEDs)
```

## Power Requirements

- **Main Supply**: 5V/3A minimum recommended
- **LED Strips**: ~60mA per LED at full white brightness
  - CSL (36 LEDs): up to 2.2A
  - LDPL (43 LEDs): up to 2.6A
- **ESP32 C3 Mini**: 3.3V internal regulation (~150mA)

**⚠️ Safety Warning**: Ensure adequate current capacity. Use proper fusing for LED power circuits.

---

## Installation

### 1. Arduino IDE Setup

1. **Install ESP32 Board Support**:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Board Manager → Search "ESP32" → Install

2. **Board Configuration**:
   - **Board**: "ESP32C3 Dev Module"
   - **USB CDC On Boot**: "Enabled"
   - **CPU Frequency**: "160MHz"
   - **Flash Mode**: "QIO"
   - **Flash Size**: "4MB (32Mb)"
   - **Partition Scheme**: "Default 4MB with spiffs"
   - **Upload Speed**: "921600"

### 2. Required Libraries

Install via Arduino Library Manager:

```
Required Libraries:
├── FastLED (by Daniel Garcia) - ⚠️ VERSION 3.9.4 REQUIRED!
└── Preferences.h (built-in ESP32 core - no installation needed)
```

### ⚠️ ESP32-C3 + FastLED Version Requirement

**IMPORTANT:** This sketch requires **FastLED 3.9.4**!

FastLED 3.9.5+ has a known bug on ESP32-C3 when using multiple LED strips:
- ESP32-C3 has only 2 RMT TX channels
- FastLED 3.9.5+ RMT driver doesn't handle this properly
- Results in "no free tx channels" error and reboot loop

**To install the correct version:**
1. Sketch → Include Library → Manage Libraries
2. Search for "FastLED"
3. Click the version dropdown and select **3.9.4**
4. Click Install

**Bug Reference:**
- [FastLED Issue #1498 - ESP32-C3 RMT Driver Bug](https://github.com/FastLED/FastLED/issues/1498)

### 3. Upload & Test

1. Connect ESP32 C3 Mini via 9600USB
2. Select correct COM port
3. Upload code (Ctrl+U)
4. Open Serial Monitor ( baud)
5. Type `help` for command list
6. LEDs should flash blue on startup

---

## Control Interfaces

### 1. Button Control

| Button | Short Press | Long Press (2s) |
|--------|-------------|-----------------|
| Button 1 (IO0) | Cycle Power State (On → Demo → Off) | - |
| Button 2 (IO1) | Next Animation | - |
| Button 3 (IO2) | Change Speed (4 levels) | - |
| Button 4 (IO3) | Next Color (17 modes) | - |
| Button 5 (IO4) | Change Brightness (5 levels) | - |
| Button 6 (IO5) | Toggle Mode (Animation ↔ Simon Game) | Save Settings to EEPROM |

### 2. Serial Monitor Control (9600 baud)

Open Serial Monitor in Arduino IDE, set line ending to "Newline".

#### Available Commands

| Command | Description | Values |
|---------|-------------|--------|
| `help` | Display command list | - |
| `status` | Show current system status | - |
| `config` | Show all current settings | - |
| `anim=<n>` | Select animation | 0-6 |
| `speed=<n>` | Set speed index | 0-3 |
| `bright=<n>` | Set brightness index | 0-4 |
| `color=<n>` | Set color mode (palette) | 0-16 (0=multi-color) |
| `rgb=R,G,B` | Set custom RGB color | 0-255 each (e.g., `rgb=255,0,128`) |
| `power=<n>` | Set power state | 0=ON, 1=OFF, 2=DEMO |
| `mode=<n>` | Set main mode | 0=Animation, 1=Simon Game |
| `save` | Save settings to EEPROM | - |
| `reset` | Restart the system | - |

#### Usage Examples

```
status           # Show current system status
config           # Display all settings
anim=3           # Select sparkle animation
speed=2          # Set medium-fast speed
bright=4         # Maximum brightness
color=0          # Multi-color mode
color=5          # Palette color 5
rgb=255,0,0      # Pure red (custom RGB)
rgb=0,255,128    # Cyan-green (custom RGB)
rgb=255,255,255  # White!
power=2          # Demo mode (auto-cycle)
mode=1           # Start Simon game
save             # Save current settings
reset            # Restart system
```

#### Example Output: `status`
```
--- System Status ---
Power: DEMO
Mode: Animation
Animation: 3 (sparkle)
Demo Override: No
Uptime: 1234 seconds
--------------------
```

#### Example Output: `config`
```
--- Current Configuration ---
Power State: 2 (DEMO)
Animation: 3 (sparkle)
Speed: 1 (1.0x)
Brightness: 2 (90)
Color Mode: 0 (Multi-color)
Main Mode: 0 (Animation)
--------------------
```

### 3. Kill-Switch

Connect a switch between IO8 and GND:
- **Switch Open**: Normal operation
- **Switch Closed**: All LEDs off, all logic bypassed

Perfect for:
- Emergency shutoff
- Transport mode
- Power saving during events

---

## Animation Modes

### Available Animations (0-6)

| Index | Name | Description |
|-------|------|-------------|
| 0 | **beatit** | VU-meter style bar animation |
| 1 | **breathe** | Smooth breathing/pulsing effect |
| 2 | **juggle** | Multiple bouncing dots |
| 3 | **sparkle** | Random sparkling effect |
| 4 | **comet** | Single moving dot with fade trail |
| 5 | **alarm** | Fast flashing alarm effect (synced CSL+LDPL) |
| 6 | **fire** | Realistic fire simulation (synced CSL+LDPL) |

### LDPL Panel Behavior

- Animations 0-4: LDPL runs independent "largelogic" effect (pulsing with periodic sweep)
- Animations 5-6 (alarm, fire): LDPL synchronized with CSL panel

---

## Parameter Settings

### Speed Levels (0-3)

| Index | Multiplier | Effect |
|-------|------------|--------|
| 0 | 0.5x | Slow/relaxed |
| 1 | 1.0x | Normal (default) |
| 2 | 1.5x | Fast |
| 3 | 2.0x | Very fast |

### Brightness Levels (0-4)

| Index | Value | Description |
|-------|-------|-------------|
| 0 | 10 | Very dim (power saving) |
| 1 | 40 | Low |
| 2 | 90 | Medium (default) |
| 3 | 160 | Bright |
| 4 | 255 | Maximum |

### Color Modes (0-16)

| Index | Description |
|-------|-------------|
| 0 | Multi-color (rainbow cycling) |
| 1-16 | Fixed hue (16 positions around color wheel) |

---

## Simon Memory Game

### How to Play

1. Press Button 6 (short press) to enter Simon mode
2. Watch the LED sequence carefully
3. Repeat the sequence by pressing the corresponding buttons (1-6)
4. Each successful round adds one more step
5. Reach level 20 to win!

### Game Feedback

- **Blue flash (2x)**: Game starting
- **Green flash (5x)**: You won!
- **Red flash (3x)**: Game over

### Controls During Game

- Buttons 1-5 map to LED segments 1-5 (6 LEDs each)
- Button 6 short press = Segment 6 input
- **Button 6 long press (1 sec) = Exit Simon Game**
- Or use CLI: `mode=0` to exit

---

## Power States

| State | Behavior |
|-------|----------|
| **ON** (0) | Selected animation plays continuously |
| **OFF** (1) | All LEDs off, system idle |
| **DEMO** (2) | Auto-cycles through all animations (10s each) |

---

## Persistent Settings

Settings are automatically loaded on boot from EEPROM:

- Power state
- Animation mode
- Speed index
- Color mode
- Brightness index

### Saving Settings

- **Via Button**: Long press Button 6 (2 seconds)
- **Via Serial**: Type `save` command
- **Confirmation**: Green flash (2x) indicates successful save

---

## Troubleshooting

### LEDs Not Working

- ✅ Check 5V power supply connection
- ✅ Verify data pin connections (IO6 for CSL, IO7 for LDPL)
- ✅ Check LED count configuration (36 for CSL, 43 for LDPL)
- ✅ Ensure GND is connected between ESP32 and LED strips
- ✅ Test with `bright=4` command for maximum brightness

### Serial Commands Not Working

- ✅ Set baud rate to **9600**
- ✅ Set line ending to **"Newline"** (not "No line ending")
- ✅ Check USB cable (some cables are charge-only)
- ✅ Verify correct COM port selected

### Buttons Not Responding

- ✅ Check button wiring (connect between IO pin and GND)
- ✅ Buttons use internal pull-up resistors
- ✅ Verify correct pin assignments (IO0-IO5)

### Kill-Switch Issues

- ✅ Switch should connect IO8 to GND when closed
- ✅ Uses internal pull-up - open = HIGH, closed = LOW
- ✅ Check for solid connection when switch is closed

### Settings Not Saving

- ✅ Wait for green flash confirmation
- ✅ Hold Button 6 for full 2 seconds
- ✅ Try `save` command via Serial Monitor
- ✅ Check for EEPROM write errors in Serial output

### Simon Game Problems

- ✅ Ensure in Simon mode (Button 6 short press to toggle)
- ✅ Press buttons firmly and release
- ✅ Watch sequence carefully before repeating
- ✅ Press Button 6 to restart after game over

---

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| Microcontroller | ESP32 C3 Mini |
| LED Type | WS2812 / NeoPixel |
| CSL Panel LEDs | 36 |
| LDPL Panel LEDs | 43 |
| Control Buttons | 6 |
| Serial Baud Rate | 9600 |
| CLI Commands | 12 (help, status, config, anim, speed, bright, color, rgb, power, mode, save, reset) |
| Color Modes | 18 (1 multi + 16 palette + 1 custom RGB) |
| Animations | 7 |
| Speed Levels | 4 |
| Brightness Levels | 5 |
| Simon Game Max Level | 20 |
| Button Debounce | 25ms |
| Long Press Threshold | 2000ms |

---

## Wiring Diagram

```
                    ESP32 C3 Mini
                   ┌─────────────┐
    Button 1 ──────┤ IO0     5V  ├────── 5V Power
    Button 2 ──────┤ IO1    GND  ├────── GND
    Button 3 ──────┤ IO2         │
    Button 4 ──────┤ IO3     IO6 ├────── CSL Data (36 LEDs)
    Button 5 ──────┤ IO4     IO7 ├────── LDPL Data (43 LEDs)
    Button 6 ──────┤ IO5     IO8 ├────── Kill-Switch
                   └─────────────┘
                         │
                        GND (common)

    Buttons: Connect between IO pin and GND
    Kill-Switch: Connect between IO8 and GND
    LEDs: Data → IO pin, +5V → Power Supply, GND → Common
```

---

## License & Credits

### Project Credits
- **Hardware Design**: Printed-Droid.com
- **Software**: Printed-Droid.com (Michael Whitley (v1.0))
- **ESP32 C3 Mini Port & Enhancements**: Printed-Droid.com

### Open Source Libraries
- **FastLED**: LED control library by Daniel Garcia
- **Preferences.h**: ESP32 core EEPROM abstraction

### Disclaimer

⚠️ **IMPORTANT SAFETY NOTICE** ⚠️

This project involves electrical components and LED strips. Users are responsible for:

- Proper electrical safety and insulation
- Adequate power supply sizing and protection
- Safe assembly and operation
- Compliance with local electrical codes
- Testing all functions before final installation

**BUILD AT YOUR OWN RISK.** Ensure proper knowledge of electronics and safety practices.

---

**May the Force be with your build!** 🌟

*For updates and community support, visit: www.printed-droid.com*
