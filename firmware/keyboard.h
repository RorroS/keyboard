#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <Arduino.h>
#include "constants.h"

namespace Ze {

    const int KEY_DUMMY = 0;

    // Codes for modifiers, but aren't actual
    // keycodes for the modifiers, but exist to
    // make things more efficient. Do not change
    // these.
    const int KEY_LSHIFT = -1;
    const int KEY_CTRL = -2;
    const int KEY_ALT = -3;
    const int KEY_ALTGR = -4;
    const int KEY_SUPER = -5;
    const int KEY_RSHIFT = -6;

    // Again, this keycode is not sent, but used
    // to control behavior of other keys
    const int KEY_FN = -7;
    const int KEY_FN2 = -8;

    const uint8_t NUM_MODIFIERS = 6;

    const uint8_t READ_DELAY = 5;

    struct Key {
        
        /*
         * Initializes dummy key.
         */
        Key();

        /*
         * Initializes key without second function.
         */
        Key(int code);

        /*
         * Initializes key with main and second function.
         */
        Key(int main, int second);

        /*
         * Initializes key with main, second and third function
         */
         Key(int main, int second, int third);

        /*
         * Checks whether this key is a modifier key.
         */
        bool is_modifier() const;

        /*
         * Checks whether this key is a dummy key.
         */
        bool is_dummy() const;

        /*
         * Checks whether this key is the FN key.
         */
        bool is_fn() const;

        /*
         * Checks if key pressed is the second FN key
         */
        bool is_fn2() const;

        /*
         * Checks whether this key has a second function.
         */
        bool has_second() const;

        /*
         * Checks if key has a third function.
         */
        bool has_third() const;

        bool operator==(const Key& other) const;

        bool operator!=(const Key& other) const;

        /* 
         * The keycode for the main function of this key.
         */
        int code;

        /*
         * The keycode for the second function of this key,
         * that is, the function this key will have when 
         * the FN-key is pressed. If this key has no second
         * function, this is set as KEY_DUMMY.
         */
        int second;

        /*
         * Third function of this key. Will be sent when 
         * the FN-2 is pressed.
         */
        int third;
    
    };

    class Board {

        public:
            
            /*
             * Initializes the keyboard, by initializing
             * the ports to the switches. 
             */
            void init();

            void update();

            Key* get_curr_pressed_keys();

            Key* get_just_released_keys();

            uint8_t get_num_keys_pressed();

            uint8_t get_num_released_keys();

        private:

            const int MODIFIER_MAP[NUM_MODIFIERS] = {

                MODIFIERKEY_LEFT_SHIFT,
                MODIFIERKEY_CTRL,
                MODIFIERKEY_LEFT_ALT,
                MODIFIERKEY_RIGHT_ALT,
                MODIFIERKEY_GUI,
                MODIFIERKEY_RIGHT_SHIFT

            };

            const uint8_t ROW_PINS[NUM_ROWS] = {3, 4, 5, 6, 7};
            const uint8_t COL_PINS[NUM_COLS] = 
            {
                8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22
            };

            bool fn_pressed;
            bool fn2_pressed;
            KeyboardState current_state;
            bool recently_state_changed;

            uint8_t num_keys_pressed;

            uint8_t num_keys_released;

            uint8_t tot_num_keys_pressed;

            /*
             * The keys that are being pressed in the
             * current iteration, which can be sent.
             * The keys always occupy the lowes possible index.
             */
            Key curr_pressed_keys[MAX_NUM_KEYS];

            /*
             * Array of all currently pressed keys, even
             * those that won't be sent.
             */
            Key all_pressed_keys[NUM_ROWS * NUM_COLS];

            /*
             * Array of the keys that were just released
             * in this iteration.
             */
            Key just_released_keys[MAX_NUM_KEYS];

            /*
             * The keys to be sent. We need this
             * to make sure that the same key is
             * not in one instance sent as for instance
             * key 1, and then key 2. 
             */
            Key keys_to_send[MAX_NUM_KEYS];

            /*
             * The actual keycodes to be sent.
             */
            int codes_to_send[MAX_NUM_KEYS];

            int current_modifier;

            /*
             * Some pointer magic to make changing modes work.
             */
            const Key (*keymap)[14];

            /*
             * Gets the real modifier code for a modifier key.
             * Does not translate the FN-key, and the modifier
             * must be negative.
             */
            int translate_modifier(const int modifier) const;

            /*
             * Removes the keys from the keys_to_send
             * and codes_to_send arrays that are no
             * longer being pressed.
             */
            void remove_released_keys();

            /*
             * Sets the recently_state_changed to false
             * if the fn keys are released.
             */
            void set_recently_state_change();

            /*
             * Clears curr_pressed_keys.
             */
            void reset_pressed_keys();

            /*
             * Places a key into a free spot in the
             * keys_to_send and codes_to_send arrays.
             * Returns true if the key was placed,
             * false if the array was full
             * or the key had already been placed.
             */
            bool try_place_key(Key& k);

            /*
             * Updates the keys_to_send
             * and codes_to_send arrays, 
             * based on an up to date
             * curr_pressed_keys array.
             */
            void update_keys_to_send();

            void scan_keys();

            void send_keys();
    
    };

    const Key KEYS[NUM_ROWS][NUM_COLS] = {

        //******************************************
        // ROW 0
        //******************************************
        
        {
            Key(KEY_BACKSPACE),
            Key(KEY_EQUAL, KEY_F12),
            Key(KEY_MINUS, KEY_F11),
            Key(KEY_0, KEY_F10), 
            Key(KEY_9, KEY_F9),
            Key(KEY_8, KEY_F8),
            Key(KEY_7, KEY_F7),
            Key(KEY_6, KEY_F6), 
            Key(KEY_5, KEY_F5),
            Key(KEY_4, KEY_F4), 
            Key(KEY_3, KEY_F3),
            Key(KEY_2, KEY_F2),
            Key(KEY_1, KEY_F1),
            Key(KEY_ESC, KEY_TILDE)
        },

        //******************************************
        // ROW 1
        //******************************************

        {
            Key(),
            Key(KEY_RIGHT_BRACE),
            Key(KEY_LEFT_BRACE),
            Key(KEY_P),
            Key(KEY_O), 
            Key(KEY_I),
            Key(KEY_U), 
            Key(KEY_Y),
            Key(KEY_T), 
            Key(KEY_R),
            Key(KEY_E, KEY_DUMMY, KEYPAD_9), 
            Key(KEY_W, KEY_DUMMY, KEYPAD_8),
            Key(KEY_Q, KEY_DUMMY, KEYPAD_7), 
            Key(KEY_TAB)
        },

        //******************************************
        // ROW 2
        //******************************************
        
        {
            Key(KEY_ENTER),
            Key(KEY_BACKSLASH),
            Key(KEY_QUOTE),
            Key(KEY_SEMICOLON),
            Key(KEY_L, KEY_UP),
            Key(KEY_K),
            Key(KEY_J),
            Key(KEY_H),
            Key(KEY_G), 
            Key(KEY_F), 
            Key(KEY_D, KEY_DUMMY, KEYPAD_6), 
            Key(KEY_S, KEY_DUMMY, KEYPAD_5),
            Key(KEY_A, KEY_DUMMY, KEYPAD_4),
            Key(KEY_CAPS_LOCK)
        },

        //******************************************
        // ROW 3
        //******************************************
        
        {
            Key(KEY_RSHIFT),
            Key(), // dummy
            Key(KEY_SLASH, KEY_RIGHT),
            Key(KEY_PERIOD, KEY_DOWN),
            Key(KEY_COMMA, KEY_LEFT),
            Key(KEY_M),
            Key(KEY_N),
            Key(KEY_B),
            Key(KEY_V),
            Key(KEY_C, KEY_DUMMY, KEYPAD_3),
            Key(KEY_X, KEY_DUMMY, KEYPAD_2),
            Key(KEY_Z, KEY_DUMMY, KEYPAD_1),
            Key(KEY_NON_US_BS, KEY_DUMMY, KEYPAD_0),
            Key(KEY_LSHIFT)
        },

        //******************************************
        // ROW 4
        //******************************************
        
        {
            Key(KEY_CTRL),
            Key(KEY_FN),
            Key(KEY_FN2),
            Key(KEY_ALTGR),
            Key(), // dummy
            Key(), // dummy
            Key(), // dummy
            Key(KEY_SPACE),
            Key(), // dummy
            Key(), // dummy
            Key(), // dummy
            Key(KEY_ALT),
            Key(KEY_SUPER),
            Key(KEY_CTRL)
        }
    };

    
     const Key KEYS_SPECIAL[NUM_ROWS][NUM_COLS] = {

        //******************************************
        // ROW 0
        //******************************************
        
        {
            Key(KEY_BACKSPACE),
            Key(KEY_F12, KEY_EQUAL),
            Key( KEY_F11, KEY_MINUS),
            Key(KEY_F10, KEY_0), 
            Key( KEY_F9, KEY_9),
            Key(KEY_F8, KEY_8),
            Key(KEY_F7, KEY_7),
            Key(KEY_F6, KEY_6), 
            Key(KEY_F5, KEY_5),
            Key(KEY_F4, KEY_4), 
            Key(KEY_F3, KEY_3),
            Key(KEY_F2, KEY_2),
            Key(KEY_F1, KEY_1),
            Key(KEY_ESC, KEY_TILDE)
        },

        //******************************************
        // ROW 1
        //******************************************

        {
            Key(),
            Key(KEY_RIGHT_BRACE),
            Key(KEY_LEFT_BRACE),
            Key(KEY_P),
            Key(KEY_O), 
            Key(KEY_I),
            Key(KEY_U), 
            Key(KEY_Y),
            Key(KEY_T), 
            Key(KEY_R),
            Key(KEY_E, KEY_DUMMY, KEYPAD_9), 
            Key(KEY_W, KEY_DUMMY, KEYPAD_8),
            Key(KEY_Q, KEY_DUMMY, KEYPAD_7), 
            Key(KEY_TAB)
        },

        //******************************************
        // ROW 2
        //******************************************
        
        {
            Key(KEY_ENTER),
            Key(KEY_BACKSLASH),
            Key(KEY_QUOTE),
            Key(KEY_SEMICOLON),
            Key(KEY_L, KEY_UP),
            Key(KEY_K),
            Key(KEY_J),
            Key(KEY_H),
            Key(KEY_G), 
            Key(KEY_F), 
            Key(KEY_D, KEY_DUMMY, KEYPAD_6), 
            Key(KEY_S, KEY_DUMMY, KEYPAD_5),
            Key(KEY_A, KEY_DUMMY, KEYPAD_4),
            Key(KEY_CAPS_LOCK)
        },

        //******************************************
        // ROW 3
        //******************************************
        
        {
            Key(KEY_RSHIFT),
            Key(), // dummy
            Key(KEY_SLASH, KEY_RIGHT),
            Key(KEY_PERIOD, KEY_DOWN),
            Key(KEY_COMMA, KEY_LEFT),
            Key(KEY_M),
            Key(KEY_N),
            Key(KEY_B),
            Key(KEY_V),
            Key(KEY_C, KEY_DUMMY, KEYPAD_3),
            Key(KEY_X, KEY_DUMMY, KEYPAD_2),
            Key(KEY_Z, KEY_DUMMY, KEYPAD_1),
            Key(KEY_NON_US_BS, KEY_DUMMY, KEYPAD_0),
            Key(KEY_LSHIFT)
        },

        //******************************************
        // ROW 4
        //******************************************
        
        {
            Key(KEY_CTRL),
            Key(KEY_FN),
            Key(KEY_FN2),
            Key(KEY_ALTGR),
            Key(), // dummy
            Key(), // dummy
            Key(), // dummy
            Key(KEY_SPACE),
            Key(), // dummy
            Key(), // dummy
            Key(), // dummy
            Key(KEY_ALT),
            Key(KEY_SUPER),
            Key(KEY_CTRL)
        }
    };
};

#endif
