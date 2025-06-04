// Copyright 2016-2022 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#include "Kaleidoscope.h"
#include "Kaleidoscope-HardwareTestMode.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-IdleLEDs.h"
#include "Kaleidoscope-LED-ActiveModColor.h"
#include "Kaleidoscope-LEDBrightnessConfig.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LEDEffect-BootGreeting.h"
#include "Kaleidoscope-LEDEffect-Rainbow.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-MagicCombo.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-NumPad.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-USB-Quirks.h"

// Macros
// To assign in a keymap: M(MACRO_NAME)  e.g. M(MACRO_ARROW)
// Also, see macroAction function
enum {
    MACRO_VERSION_INFO,
    MACRO_ARROW,
};


/**
  * `Key_` definitions here:
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs_keyboard.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs_consumerctl.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs_sysctl.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs_keymaps.h
  *
  *  - ___ will let keypresses fall through to the previously active layer
  *  - XXX will mark a keyswitch as 'blocked' on this layer
  *  - ShiftToLayer() switch to layer while held
  *  - LockLayer() change to layer when pressed
  *
  * PROG key docs:
  * https://community.keyboard.io/t/how-the-prog-key-gets-you-into-the-bootloader/506/8
  */

// Note: layers are 0-indexed
enum {
    PRIMARY,
    SYMBOL,
    MOUSE,
};

// Aliases for readability
#define Key_LeftCurly    Key_LeftCurlyBracket
#define Key_RightCurly   Key_RightCurlyBracket


KEYMAPS(

  [PRIMARY] = KEYMAP_STACKED(
          // Left Hand
          Key_Equals,    Key_1, Key_2, Key_3, Key_4, Key_5, Key_Escape,
          Key_CapsLock,  Key_Q, Key_W, Key_E, Key_R, Key_T, ___,
          Key_Tab,       Key_A, Key_S, Key_D, Key_F, Key_G,
          Key_LeftShift, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Meh,

          CTL_T(Backspace), ALT_T(Delete), GUI_T(Home), Key_End,
          ShiftToLayer(SYMBOL),


          // Right Hand
          ___,                 Key_6, Key_7, Key_8,     Key_9,         Key_0,         Key_Minus,
          ___,                 Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Backslash,
                               Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
          ShiftToLayer(MOUSE), Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     Key_RightShift,

          Key_PageDown, GUI_T(PageUp), ALT_T(Enter), CTL_T(Spacebar),
          ShiftToLayer(SYMBOL)
    ),


  [SYMBOL] =  KEYMAP_STACKED(
          Key_Backtick,  Key_F1,  Key_F2,  Key_F3,  Key_F4,  Key_F5,  M(MACRO_VERSION_INFO),
          ___, ___, ___, ___, ___, ___, ___,
          ___, ___, ___, ___, ___, ___,
          ___, ___, ___, ___, ___, ___, ___,
          ___, ___, ___, ___,
          ___,

          ___,          Key_F6,        Key_F7,        Key_F8,         Key_F9,          Key_F10,          ___,
          ___,          ___,           Key_LeftCurly, Key_RightCurly, Key_LeftBracket, Key_RightBracket, ___,
                        Key_LeftArrow, Key_DownArrow, Key_UpArrow,    Key_RightArrow,  ___,              ___,
          Key_LeftGui,  ___,           ___,           Key_8,          M(MACRO_ARROW),  ___,              ___,
          ___, ___, ___, ___,
          ___),


  [MOUSE] =  KEYMAP_STACKED(
          ___, ___, ___,          ___,         ___,          ___, ___,
          ___, ___, Key_mouseUpL, Key_mouseUp, Key_mouseUpR, ___, ___,
          ___, ___, Key_mouseL,   Key_mouseDn, Key_mouseR,   ___,
          ___, ___, Key_mouseDnL, ___,         Key_mouseDnR, ___, ___,
          ___, ___, ___, ___,
          ___,

          ___, ___,              ___,             ___,             ___,           ___,           ___,
          ___, Key_mouseWarpEnd, Key_mouseWarpNW, Key_mouseWarpNE, Key_mouseBtnL, Key_mouseBtnR, ___,
               ___,              Key_mouseWarpSW, Key_mouseWarpSE, ___,           ___,           ___,
          ___, ___,              ___,             ___,             ___,           ___,           ___,
          ___, ___, ___, ___,
          ___),


  // Template
  // [LAYER_NAME] =  KEYMAP_STACKED
  // (___, ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___,
  //  ___,

  //  ___, ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___, ___, ___, ___,
  //       ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___, ___, ___, ___,
  //  ___, ___, ___, ___,
  //  ___),


) // KEYMAPS


#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built on " __DATE__ " at " __TIME__
#endif
#define XSTR(s) STR(s)
#define STR(s) #s


static const macro_t *versionInfoMacro(uint8_t key_state) {
    if (keyToggledOn(key_state)) {
        return Macros.type(PSTR("Keyboardio Model 100 - Firmware version "),
                           PSTR(XSTR(BUILD_INFORMATION)));
    }
}

static const macro_t *arrowOperatorMacro(uint8_t keyState) {
    if (keyToggledOn(keyState)) {
        return Macros.type(PSTR("->"));
    }
}


// macro dispatch
// - macro_id: see MACRO_* enum above
// - event: see Kaleidoscope/src/kaleidoscope/KeyEvent.h
const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
    switch (macro_id) {
    case MACRO_VERSION_INFO:
        return versionInfoMacro(event.state);
        break;

    case MACRO_ARROW:
        return arrowOperatorMacro(event.state);
        break;
    }
    return MACRO_NONE;
}

// toggle the LEDs off when the host goes to sleep, and turn them back on when it wakes up.
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
    switch (event) {
    case kaleidoscope::plugin::HostPowerManagement::Suspend:
    case kaleidoscope::plugin::HostPowerManagement::Sleep:
        LEDControl.disable();
        break;
    case kaleidoscope::plugin::HostPowerManagement::Resume:
        LEDControl.enable();
        break;
    }
}

// dispatch power management events (suspend, resume, and sleep)
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
    toggleLedsOnSuspendResume(event);
}

// See USE_MAGIC_COMBOS call below.
enum {
    // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
    // mode.
    COMBO_TOGGLE_NKRO_MODE,
    // Enter test mode
    COMBO_ENTER_TEST_MODE
};

// toggle the keyboard protocol via USBQuirks
static void toggleKeyboardProtocol(uint8_t combo_index) {
    USBQuirks.toggleKeyboardProtocol();
}

// enter the hardware test mode
static void enterHardwareTestMode(uint8_t combo_index) {
    HardwareTestMode.runTests();
}

USE_MAGIC_COMBOS(
        [COMBO_TOGGLE_NKRO_MODE] = {
            .action = toggleKeyboardProtocol,
            .keys = {R3C6, R2C6, R3C7}  // Left Fn + Esc + Shift
        },

        [COMBO_ENTER_TEST_MODE] = {
            .action = enterHardwareTestMode,
            .keys = {R3C6, R0C0, R0C6}  // Left Fn + Prog + LED
        }
);

// Init order is important.
KALEIDOSCOPE_INIT_PLUGINS(

        // pulse the LED button when the keyboard is starting
        BootGreetingEffect,

        // For primary/secondary actions on tap/hold
        Qukeys,

        HardwareTestMode,

        // LED stuff
        LEDControl,
        LEDRainbowWaveEffect,
        ActiveModColorEffect,
        IdleLEDs,

        // See macroAction, above
        // https://kaleidoscope.readthedocs.io/en/latest/plugins/Kaleidoscope-Macros.html
        Macros,

        MouseKeys,

        // see hostPowerManagementEventHandler
        // https://kaleidoscope.readthedocs.io/en/latest/plugins/Kaleidoscope-HostPowerManagement.html
        HostPowerManagement,

        // See USE_MAGIC_COMBOS
        // https://kaleidoscope.readthedocs.io/en/latest/plugins/Kaleidoscope-MagicCombo.html
        MagicCombo,

        // Low-level workarounds for USB stuff.
        // https://kaleidoscope.readthedocs.io/en/latest/plugins/Kaleidoscope-USB-Quirks.html
        USBQuirks

);

// Standard Arduino sketch function, called when your keyboard first powers up.
// Set up Kaleidoscope and any plugins.
void setup() {
    // First, call Kaleidoscope's internal setup function
    Kaleidoscope.setup();

    // greenish
    BootGreetingEffect.hue = 85;

    LEDRainbowWaveEffect.brightness(100);

    // Set the action key the test mode should listen for to Left Fn
    HardwareTestMode.setActionKey(R3C6);

    LEDOff.activate();

    // 80% is the default; set here for reference
    // Increase this value if getting too many non-modified keys
    // Decrease this value if getting too many modified keys
    Qukeys.setOverlapThreshold(80);
}

// Standard Arduino sketch function.  Event loop.
// For Kaleidoscope-based keyboard firmware, you usually just want to
// call Kaleidoscope.loop(); and not do anything custom here.
void loop() {
    Kaleidoscope.loop();
}
