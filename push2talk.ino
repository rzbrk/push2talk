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
 *   - Implement config mode to change between pre-defined keyboard sequences.
 *     For now, only one sequence is implemented (CTRL+M, CTRL+M)
 * 
 */

#include <Bounce.h>

// Define buttons, more specific, to which pin the buttons are connected to
int p2t_pin = 6;      // push-to-talk (p2t) button
int conf_pin = 7;     // config (conf) button

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
 * Configure the IO ports for the buttons as inputs using the internal
 * pullup resistors.
 *
 * GND ------+------------------+
 *           |                  |
 *           +                  +
 *           \                  \
 *            \ p2t              \ conf
 *           +                  +
 *           |                  |
 *           |                  |
 *           o                  o
 *        p2t_pin           conf_pin
 * 
 */
  pinMode(p2t_pin, INPUT_PULLUP);
  pinMode(conf_pin, INPUT_PULLUP);
}

//
int sequence_switcher(int sequence, char mode, char edge) {
  int seq_total = 2; // total number of sequnces defined

  // mode == p (permutate)
  if (mode == 'p') {
    sequence = sequence + 1;
    sequence = sequence % seq_total;
    switch (sequence) {
      case 0:
        Keyboard.println("Using Sequence CTRL+m CTRL+m");
        break;
      case 1:
        Keyboard.println("Using Sequence WindowsKey+F4 WindowsKey+F4");
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
    }
  }
  return sequence;
}

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

void loop() {
  // Update all buttons
  p2t_button.update();
  conf_button.update();

  // Check for falling edges (key pressed events)
  if (p2t_button.fallingEdge()) {
    //send_win_f4__win_f4('f');
    //send_ctrl_m__ctrl_m('f');
    sequence = sequence_switcher(sequence, 'e', 'f');
  }
  if (conf_button.fallingEdge()) {
    // Do nothing here, change configuration only if we sense a rising
    // edge of the config button.
  }

  // Check for rising edges (key release event)
  if (p2t_button.risingEdge()) {
    //send_win_f4__win_f4('r');
    //send_ctrl_m__ctrl_m('r');
    sequence = sequence_switcher(sequence, 'e', 'r');
  }
  if (conf_button.risingEdge()) {
    sequence = sequence_switcher(sequence, 'p', 'r');
  }
}
