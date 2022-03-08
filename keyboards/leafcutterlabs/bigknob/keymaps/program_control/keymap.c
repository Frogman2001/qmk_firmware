/* Copyright 2021 Craig Gardner
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

 /*

----- Controls -----

 - Wheel rotate:    Volume up/down
 - Wheel press:     Mute / unmute
 - Key 4:           opera
 - Key 3:           chrome, n taps rotate through
 - Key 2:           teams, n taps rotate through
 - Key 1:           calendar, doubletap email

 Build command:  qmk compile -kb leafcutterlabs/bigknob -km program_control

 */

#include QMK_KEYBOARD_H

#define _MAIN 0


enum win_combos {
    Wheel_tap,
    KEY_4,      // represents the key next to the wheel
    KEY_3,
    KEY_2,
    KEY_1       // represents the key at the end, away from the wheel
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = { //button closest to USB is first
    [_MAIN] = LAYOUT(
        TD(Wheel_tap), TD(KEY_4), TD(KEY_3), TD(KEY_2), TD(KEY_1)
    )
};


bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return true;
}


void rotate_program_selection(uint16_t count, uint16_t selector_keycode)
{
    int counter;

    register_code (KC_LWIN);

    for (counter = 1; counter <= count; counter = counter + 1 )
    {
        register_code (selector_keycode);
        _delay_ms(65);                          // pause so that the popup of open windows can render and allow one to be selected
        unregister_code (selector_keycode);
    }

    unregister_code (KC_LWIN);
}


// https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/
void dance_select_outlook_item(qk_tap_dance_state_t *state, void *user_data) {

    rotate_program_selection(1, KC_3);          // go to first outlook item - probably main window
    _delay_ms(150);

    switch (state->count)
    {
        case 1:
            send_string(SS_LCTRL(SS_TAP(X_2)));  // select calendar
            break;

        case 2:
            send_string(SS_LCTRL(SS_TAP(X_1)));  // select mail
            break;
    }
}


void dance_select_teams_item(qk_tap_dance_state_t *state, void *user_data) {
    rotate_program_selection(state->count, KC_4);
}


void dance_select_chrome_item(qk_tap_dance_state_t *state, void *user_data) {
    rotate_program_selection(state->count, KC_6);
}


void dance_rgb_finished (qk_tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        send_string(SS_LWIN("5"));
    } else if (state->count == 2) {
        rgblight_toggle();
    } else if (state->count == 3) {
        rgblight_step();
    }
}


void dance_mute_video(qk_tap_dance_state_t *state, void *user_data) {
    rotate_program_selection(2, KC_4);          // go to first teams item - probably main window
    _delay_ms(150);

    if (state->count == 1) {
        send_string(SS_LCTRL(SS_LSFT("m")));
    } else if (state->count == 2) {
        send_string(SS_LCTRL(SS_LSFT("o")));
    }
}


// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_tap_dance.md
qk_tap_dance_action_t tap_dance_actions[] = {
    [KEY_1] = ACTION_TAP_DANCE_FN (dance_select_outlook_item),
    [KEY_2] = ACTION_TAP_DANCE_FN (dance_select_teams_item),
    [KEY_3] = ACTION_TAP_DANCE_FN (dance_select_chrome_item),
    [KEY_4] = ACTION_TAP_DANCE_FN (dance_rgb_finished),
    [Wheel_tap] = ACTION_TAP_DANCE_FN (dance_mute_video)
};


// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_macros.md#:~:text=sloc)%2014.8%20KB-,Macros,or%20even%20help%20you%20code.
// https://github.com/qmk/qmk_firmware/blob/master/quantum/send_string_keycodes.h
/*
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case Wheel_tap:
            if (record->event.pressed) {
                //tap_code(KC_MUTE);        // turns off speakers
                //send_string(SS_LCTRL(SS_LSFT("m")));

                send_string(SS_LWIN("x"));  // same as right click Windows menu
                _delay_ms(50);

                tap_code(KC_B);             // select mobility center
                _delay_ms(500);             // wait for render

                tap_code(KC_M);             // toggle mute
                _delay_ms(1000);            // pause briefly so you can see what the current state is

                tap_code(KC_ESC);           // dismiss window
            }
            return true;
        default:
            return true;                    //Process all other keycodes normally
    }
};
*/
