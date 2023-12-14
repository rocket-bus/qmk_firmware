/* Copyright 2020 noroadsleft
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
#include QMK_KEYBOARD_H
// #include "keymap_japanese.h"

// #define L1_ENT LT(1,KC_ENT)
// #define L2_DEL LT(2,KC_DEL)
// #define L3_SPC LT(3,KC_SPC)
#define S_SLSH MT(MOD_RSFT,KC_SLSH)

#define C_TAB MT(MOD_LCTL,KC_TAB)
#define S_SCLN S(KC_SCLN)
#define S_MINS S(KC_MINS) 

enum layer_names {
    _BASE,
    _LOWER,
    _RAISE,
    _ADJUST,
    _CURSOL,
    _FUNCTION
};

static bool lower_pressed = false;  // Lowerキー記憶用
static bool raise_pressed = false;  // Raiseキー記憶用
static bool adj_pressed   = false;  // Adjustキー記憶用
static bool cur_pressed   = false;  // Cursolキー記憶用
static uint16_t lower_pressed_time = 0; // Lowerキー押し下げ時間
static uint16_t raise_pressed_time = 0; // Raiseキー押し下げ時間
static uint16_t adj_pressed_time = 0; // Adjustキー押し下げ時間
static uint16_t cur_pressed_time = 0; // cursolキー押し下げ時間
static bool is_alttab_active = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_LCTL,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_MINS, KC_QUOT,
        SFT_T(KC_LNG2), KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  S_SLSH,
                 LGUI_T(KC_ESC), LALT_T(KC_LNG1), KC_F23,  KC_F21,           KC_F21,  KC_F22, KC_F20
    ),
    [_LOWER] = LAYOUT(
        KC_F19, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5),                KC_P7, KC_P8, KC_P9, KC_PPLS, KC_PAST, LCA(KC_DEL),
        _______, S(KC_MINS),  S(KC_BSLS), KC_UP, S(KC_LBRC), S(KC_RBRC),    KC_P4, KC_P5, KC_P6, KC_PMNS,  KC_PSLS,    _______,
        _______, C(KC_PGUP), KC_LEFT, KC_DOWN, KC_RGHT,  C(KC_PGDN),        KC_P1, KC_P2, KC_P3, A(KC_F4), KC_CALC,
                 _______, _______, _______, KC_ENT,                         KC_P0, KC_PDOT, _______
    ),
    [_RAISE] = LAYOUT(
        _______, KC_1,    KC_2,   KC_3,   KC_4,   KC_5,       KC_6,   KC_7,   KC_8,   KC_9,   KC_0, LCA(KC_DEL),
        _______, KC_MINS, KC_EQL, KC_BSLS, KC_LBRC, KC_RBRC,  KC_BSLS, KC_MINS, KC_EQL, S(KC_9), S(KC_0), _______,
        _______, S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0), S(KC_BSLS), S(KC_MINS), S(KC_EQL), SGUI(KC_LEFT), SGUI(KC_RGHT),
                 _______, _______, _______, _______,          KC_ENT, _______, _______
    ),
    [_ADJUST] = LAYOUT(
        _______, _______,   _______,   _______,   _______,      _______,   _______, KC_MINS, S(KC_MINS), S(KC_MINS),   KC_EQL, KC_F1,
        _______, _______,   _______,   KC_F6,   KC_F7,  KC_F8,  KC_F9, KC_F10, KC_F11, KC_F12, _______, _______,
        _______, KC_F6,     KC_F5,     KC_F4, KC_F3, KC_F2,     KC_LEFT,  KC_RGHT, KC_UP, KC_DOWN, _______,
                 _______, _______, _______, _______,            KC_ENT, _______, _______
    ),
    [_CURSOL] = LAYOUT(
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_LPRN, S(KC_HOME), _______, S(KC_END),  _______, KC_PGUP,
        _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_LBRC, KC_HOME, KC_UP, KC_END, _______, KC_PGDN,
        _______, _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______,
                 _______, _______, _______, KC_ENT,          _______, _______, _______
    ),
    [_FUNCTION] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                 _______, _______, _______, _______,          _______, _______, _______
    )
};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint16_t lower_diff_time = 0;
    uint16_t raise_diff_time = 0;
    uint16_t adj_diff_time = 0;
    uint16_t cur_diff_time = 0;
    switch(keycode){

        case KC_F23:   // Lower/SPACEキー押下時
         if (record->event.pressed) {
             lower_pressed = true;  // Lowerキーが押されたことを記憶
             lower_pressed_time = record->event.time;

             layer_on(_LOWER);
         } else {
             layer_off(_LOWER);

             lower_diff_time = TIMER_DIFF_16(record->event.time, lower_pressed_time);

            if (lower_pressed && lower_diff_time < TAPPING_TERM) {
               register_code(KC_SPC);
               unregister_code(KC_SPC);
            }
            else if(is_alttab_active) {
                is_alttab_active = false; 
                unregister_code(KC_LALT);
            }
         lower_pressed = false;
        }
        return false;
        break;

        case KC_F22:   // RAISE/BackSpaceキー押下時
         if (record->event.pressed) {
             raise_pressed = true;  // Raiseキーが押されたことを記憶
             raise_pressed_time = record->event.time;

             layer_on(_RAISE);
         } else {
             layer_off(_RAISE);

             raise_diff_time = TIMER_DIFF_16(record->event.time, raise_pressed_time);

            if (raise_pressed && raise_diff_time < TAPPING_TERM) {
               register_code(KC_BSPC);
               unregister_code(KC_BSPC);
            }
            raise_pressed = false;
        }
        return false;
        break;

        case KC_F21:   // ADJUST/Enterキー押下時
         if (record->event.pressed) {
             adj_pressed = true;  // Adjustキーが押されたことを記憶
             adj_pressed_time = record->event.time;

             layer_on(_ADJUST);
         } else {
             layer_off(_ADJUST);

             adj_diff_time = TIMER_DIFF_16(record->event.time, adj_pressed_time);

             if (adj_pressed && adj_diff_time < TAPPING_TERM) {
                register_code(KC_ENT);
                unregister_code(KC_ENT);
             }
            adj_pressed = false;
        }
        return false;
        break;

        case KC_F20:   // CURSOL/Delキー押下時
         if (record->event.pressed) {
             cur_pressed = true;  // Cursolキーが押されたことを記憶
             cur_pressed_time = record->event.time;

             layer_on(_CURSOL);
         } else {
             layer_off(_CURSOL);

             cur_diff_time = TIMER_DIFF_16(record->event.time, cur_pressed_time);

            if (cur_pressed && cur_diff_time < TAPPING_TERM) {
               register_code(KC_DEL);
               unregister_code(KC_DEL);
            }
            cur_pressed = false;
        }
        return false;
        break;

          // TABキー（_CMDレイヤーでAltTabを発動するキー）
        case KC_F19:
            // キーが押されたとき
            if (record->event.pressed) {
                is_alttab_active = true; // AltTabのフラグをオン
                register_code(KC_LALT); // Altを押しっぱなしにする
                register_code(KC_TAB); // TABを押す

            // キーを放したとき
            } else {
                unregister_code(KC_TAB); // TABを放す
            }
            break;

        default:
         if (record->event.pressed) {
             // reset the flag
             lower_pressed = false;
             raise_pressed = false;
             adj_pressed   = false;
             cur_pressed   = false;
             is_alttab_active = false;
         }
         break;
    }
    return true;
}

// RGBLayer setting
const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS({1, 1, HSV_RED});
const rgblight_segment_t PROGMEM my_layer2_layer[] = RGBLIGHT_LAYER_SEGMENTS({1, 1, HSV_BLUE});
const rgblight_segment_t PROGMEM my_layer3_layer[] = RGBLIGHT_LAYER_SEGMENTS({1, 1, HSV_GREEN});
const rgblight_segment_t PROGMEM my_layerCL_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_YELLOW});
const rgblight_segment_t PROGMEM my_layerNL_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_MAGENTA});
const rgblight_segment_t PROGMEM my_layerIOFF_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, HSV_OFF});
const rgblight_segment_t PROGMEM my_layerLOFF_layer[] = RGBLIGHT_LAYER_SEGMENTS({1, 1, HSV_OFF});

const rgblight_segment_t * const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_layerNL_layer,
    my_layerCL_layer,
    my_layer3_layer,
    my_layer2_layer,
    my_layer1_layer,
    my_layerIOFF_layer,
    my_layerLOFF_layer
);

void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
    rgblight_sethsv_noeeprom(0, 0, 0);
};

// LayerIndicator 
layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(6, layer_state_cmp(state, 0));
    rgblight_set_layer_state(4, layer_state_cmp(state, 1));
    rgblight_set_layer_state(3, layer_state_cmp(state, 2));
    rgblight_set_layer_state(2, layer_state_cmp(state, 3));

    return state;
};

// LockIndicator
bool led_update_user(led_t led_state) {
    //rgblight_set_layer_state(0, led_state.num_lock);
    rgblight_set_layer_state(1, led_state.caps_lock);
    return true;
};

//CAPS WORD
void caps_word_set_user(bool active) {
    if (active) {
        rgblight_sethsv_range(HSV_YELLOW, 0, 1);
    } else {
        rgblight_sethsv_range(HSV_OFF, 0, 1);
    }
}