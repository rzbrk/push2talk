/*
 * Push2Talk USB HID 
 * 
 * DESCRIPTION:
 * Sends pre-defined keyboard sequences over USB to connected computer when
 * key(s) are pressed/released connected to specific pins of microcontroller.
 * 
 * VERSION:
 * v0.2
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

#include <Bounce.h>

// Define buttons, more specific, to which pin the buttons are connected to
int p2t_pin = 6;      // push-to-talk (p2t) button
int conf_pin = 7;     // config (conf) button
// Define the pin where the LED is attached to
int led_pin = 8;

// Create and initialize a variable holding the selected keyboard sequnce.
// Later, this variable is used to specify the sequence to use from a list
// of predefined sequnces.
int sequence = 0;

// Create bounce objects to debounce buttons. Debouncing time t_debounce is
// in milliseconds
int t_debounce = 10;
Bounce p2t_button = Bounce(p2t_pin, t_debounce);
Bounce conf_button = Bounce(conf_pin, t_debounce);

void setup() {
/*

 * output.
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
 *        p2t_pin           conf_pin           led_pin
 * 
 */

  pinMode(p2t_pin, INPUT_PULLUP);
  pinMode(conf_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);
  // For now, use the LED as power indicator. Therefore, just set is to
  // high.
  digitalWrite(led_pin, HIGH);
}

// This is the sequnce_switcher function. It has two modes:
//   - mode "p" (permutate): Switch to next keyboard sequence
//   - mode "s" (send):      Send the keyboard sequence. In this mode the edge
//                           type (falling/rising) is used. Depending on the
//                           selected sequence the corresponding function is
//                           called.
// Important: The variable seq_total should match with the defined keyboard
// sequnces. Each keyboard sequence should be defined in a separate function.
// If updating the sequences the two switch-case statements should be updated
// accordingly.
void sequence_switcher(char mode, char edge) {
  int seq_total = 3; // total number of sequences defined

  // mode == p (permutate)
  if (mode == 'p') {
    sequence = sequence + 1;
    sequence = sequence % seq_total;
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
        Keyboard.print("Using Sequence Windows");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.print("F4 Windows");
        Keyboard.press(KEYPAD_PLUS);
        Keyboard.release(KEYPAD_PLUS);
        Keyboard.println("F4");
        break;
      case 2:
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
        send_win_f4__win_f4(edge);
        break;
      case 2:
        send_ctrl_c__ctrl_v(edge);
    }
  }
}

// Following, the keyboard sequences are defined in dedicated functions. More
// precicely, a pair of keyboard sequences can be defined, one for the falling
// edge (key press event) or for the rising edge (key release event).

// Many programs for web conferencing using shortcut CTRL+M for both
// mute and unmute
void send_ctrl_m__ctrl_m(char edge) {
  // for both falling and rising edge send the same keyboard sequence
  if ((edge == 'f') || (edge == 'r')) {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();
    Keyboard.set_key1(KEY_M);
    Keyboard.send_now();
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
}

// Skye for Business uses WindowsKey+F4, both for mute and unmute
void send_win_f4__win_f4(char edge) {
  if ((edge == 'f' || edge == 'r')) {
    Keyboard.set_modifier(MODIFIERKEY_GUI); // WindowsKey
    Keyboard.send_now();
    Keyboard.set_key1(KEY_F4);
    Keyboard.send_now();
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
    Keyboard.set_key1(KEY_C);
    Keyboard.send_now();
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }
  if (edge == 'r') {
    Keyboard.set_modifier(MODIFIERKEY_CTRL);
    Keyboard.send_now();
    Keyboard.set_key1(KEY_V);
    Keyboard.send_now();
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
