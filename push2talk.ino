/*
 * Push2Talk USB HID 
 * 
 * DESCRIPTION:
 * Sends pre-defined keyboard sequences over USB to connected computer when
 * key(s) are pressed/released connected to specific pins of microcontroller.
 * 
 * VERSION:
 * v0.3
 * 
 * AUTHOR:
 * (c) Jan Grosser, email@jan-grosser.de
 * 
 * LICENSE:
 * GNU General Public License v3.0
 * 
 * REQUIREMENTS:
 * You need an microcontroller board featuring an ATmega32U4 equipped with
 * a full-speed USB transceiver. Only this microcontroller can emulate a
 * keyboard, mouse or joystick. Compatible microcontroller boards are:
 * 
 *   - Arduino Pro Micro
 *   - PRJC Teensy 2.0
 *   
 * For example, an Arduino Nano with ATmega328 microcontroller can NOT be
 * used.
 * 
 * AUXILIARY CIRCUIT:
 * Minimum auxiliary circuit is necessary. Only connect two push buttons to
 * two input pins of the microcontroller board as depicted below.
 * 
 * TODO/BUGS:
 *   - Fix issues with keyboard layout. HID.cpp uses US keyboard as default.
 *     This causes issues with using Keyboard.println() in this program.
 */

#include <Bounce2.h>
#include <EEPROM.h>

// Define buttons, more specific, to which pin the buttons are connected to
#define P2T_PIN 6        // push-to-talk (p2t) button
#define CONF_PIN 7       // config (conf) button
// Define the pin where the LED is attached to
#define LED_PIN 8

// Create and define a constant holding the total number of defined
// sequnces. This variable MUST match the number of defined sequences!
#define SEQ_TOTAL 4

// Create and define the EEPROM address where the selected keyboard
// sequence ist stored. This way, the last sequence can be recovered after a
// power cycle.
#define EE_ADDR 0

// Create a variable holding the selected keyboard sequence.
// Later in the setup routine this variable is initialized by reading from
// the above EEPROM address.
int sequence = 0;

// Create bounce objects to debounce buttons.
Bounce p2t_button = Bounce();
Bounce conf_button = Bounce();

// Define debouncing time in ms
#define T_DEBOUNCE 10

// Define minimum time a key has to be pressed in ms
#define t_min_pressed 100

// Setup routine
void setup() {
/*
 *
 * GND ------+------------------+-----------------+
 *           |                  |                 |
 *           +                  +                +++
 *           \                  \                | |
 *            \ p2t              \ conf          | | R_led
 *           +                  +                +++
 *           |                  |                 |
 *           |                  |               --+--
 *           |                  |                / \
 *           |                  |               --+--
 *           |                  |                 |
 *           |                  |                 |
 *           o                  o                 o
 *        P2T_PIN           CONF_PIN           LED_PIN
 * 
 */

  pinMode(P2T_PIN, INPUT_PULLUP);
  pinMode(CONF_PIN, INPUT_PULLUP);
  
  // Setuo the bounce objects
  p2t_button.attach(P2T_PIN);
  p2t_button.interval(T_DEBOUNCE);
  conf_button.attach(CONF_PIN);
  conf_button.interval(T_DEBOUNCE);

  pinMode(LED_PIN, OUTPUT);
  // For now, use the LED as power indicator. Therefore, just set is to
  // high.
  digitalWrite(LED_PIN, HIGH);

  // Read the selected sequence saved in the EEPROM
  sequence = EEPROM.read(EE_ADDR) % SEQ_TOTAL;
}

// This is the sequence_switcher function. It has two modes:
//   - mode "p" (permutate): Switch to next keyboard sequence
//   - mode "s" (send):      Send the keyboard sequence. In this mode the edge
//                           type (falling/rising) is used. Depending on the
//                           selected sequence the corresponding function is
//                           called.
// Important: The constant SEQ_TOTAL should match with the defined keyboard
// sequnces. Each keyboard sequence should be defined in a separate function.
// If updating the sequences the two switch-case statements should be updated
// accordingly.
void sequence_switcher(char mode, char edge) {

  // mode == p (permutate)
  if (mode == 'p') {
    // Increase sequence
    sequence = sequence + 1;
    sequence = sequence % SEQ_TOTAL;

    // Save current sequence to EEPROM
    EEPROM.write(EE_ADDR, sequence);

    switch (sequence) {
      case 0:
        Keyboard.print("Using Sequence CTRL");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.print("m CTRL");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.println("m");
        break;
      case 1:
        Keyboard.print("Using Sequence ALT");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.print("m ALT");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.println("m");
        break;
      case 2:
        Keyboard.print("Using Sequence Windows");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.print("F4 Windows");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.println("F4");
        break;
      case 3:
        Keyboard.print("Using Sequence CTRL");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.print("c CTRL");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.println("v");
        break;
    }
  }
  // mode == e (execute)
  if (mode == 'e') {
    switch (sequence) {
      case 0:
        send_ctrl_m__ctrl_m(edge);
        break;
      case 1:
        send_alt_m__alt_m(edge);
        break;
      case 2:
        send_win_f4__win_f4(edge);
        break;
      case 3:
        send_ctrl_c__ctrl_v(edge);
        break;
    }
  }
}

// Following, the keyboard sequences are defined in dedicated functions. More
// precicely, a pair of keyboard sequences can be defined, one for the falling
// edge (key press event) or for the rising edge (key release event).

// Many programs for web conferencing using shortcut CTRL+M for both
// mute and unmute, for instance Jitsi
void send_ctrl_m__ctrl_m(char edge) {
  // for both falling and rising edge send the same keyboard sequence
  if ((edge == 'f') || (edge == 'r')) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_key1(KEY_M);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
}

// BigBlueButton unses shortcut ALT+m for both mute and unmute
void send_alt_m__alt_m(char edge) {
  if ((edge == 'f') || (edge == 'r')) {
    Keyboard.set_modifier(MODIFIERKEY_ALT);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_key1(KEY_M);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
}

// Skye for Business uses WindowsKey+F4, both for mute and unmute
void send_win_f4__win_f4(char edge) {
  if ((edge == 'f') || (edge == 'r')) {
    Keyboard.set_modifier(MODIFIERKEY_GUI); // WindowsKey
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_key1(KEY_F4);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
}

// Using the button for copy-paste
void send_ctrl_c__ctrl_v(char edge) {
  if (edge == 'f') {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_key1(KEY_C);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
  if (edge == 'r') {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_key1(KEY_V);
    Keyboard.send_now();
    delay(t_min_pressed);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
}

// Main loop
void loop() {
  // Update all buttons
  p2t_button.update();
  conf_button.update();

  // Check for falling edges (key pressed events)
  if (p2t_button.fallingEdge()) {
    //send_win_f4__win_f4('f');
    //send_ctrl_m__ctrl_m('f');
    sequence_switcher('e', 'f');
  }
  if (conf_button.fallingEdge()) {
    // Do nothing here, change configuration only if we sense a rising
    // edge of the config button.
  }

  // Check for rising edges (key release event)
  if (p2t_button.risingEdge()) {
    //send_win_f4__win_f4('r');
    //send_ctrl_m__ctrl_m('r');
    sequence_switcher('e', 'r');
  }
  if (conf_button.risingEdge()) {
    sequence_switcher('p', 'r');
  }
}
