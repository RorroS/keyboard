#include "keyboard.h"


Ze::Key::Key() {
    this->code = KEY_DUMMY;
}

Ze::Key::Key(int code) {
    this->code = code;
    this->second = KEY_DUMMY;
    this->third = KEY_DUMMY;
}

Ze::Key::Key(int code, int second) {
    this->code = code;
    this->second = second;
    this->third = KEY_DUMMY;
}

Ze::Key::Key(int code, int second, int third) {
    this->code = code;
    this->second = second;
    this->third = third;
}

bool Ze::Key::is_modifier() const {
    return this->code < 0;
}

bool Ze::Key::is_media() const {
    return (0xFF00 & this->code) == 0xE400;
}

bool Ze::Key::is_dummy() const {
    return code == KEY_DUMMY;
}

bool Ze::Key::operator==(const Key& other) const {
    return code == other.code && second == other.second;
}

bool Ze::Key::operator!=(const Key& other) const {
    return !(*this == other);
}

bool Ze::Key::is_fn() const {
    return code == KEY_FN;
}

bool Ze::Key::is_fn2() const {
    return code == KEY_FN2;
}

bool Ze::Key::has_second() const {
    return second != KEY_DUMMY;
}

bool Ze::Key::has_third() const {
    return third != KEY_DUMMY;
}

int Ze::Board::translate_modifier(const int modifier) const {
    if (modifier >= 0) return -1;
    return MODIFIER_MAP[-1 * modifier - 1];
}


void Ze::Board::init() {

    // Set the rows as outputs
    for (uint8_t r = 0; r < NUM_ROWS; ++r) {
        pinMode(ROW_PINS[r], OUTPUT);
        digitalWrite(ROW_PINS[r], HIGH);
    }

    // Set the columns as inputs with pull up resistors
    for (uint8_t c = 0; c < NUM_COLS; ++c) {
        pinMode(COL_PINS[c], INPUT_PULLUP);
        digitalWrite(COL_PINS[c], INPUT_PULLUP);
    }

    fn_pressed = false;
    fn2_pressed = false;
    num_keys_pressed = 0;
    tot_num_keys_pressed = 0;
    current_media = 0;

    // Initialize the key buffers to nullptrs
    for (uint8_t i = 0; i < MAX_NUM_KEYS; ++i) {
        curr_pressed_keys[i] = Key();
        keys_to_send[i] = Key();
        codes_to_send[i] = 0;
    }

    // Initialize the all_pressed_keys array.
    for (uint8_t i = 0; i < NUM_ROWS * NUM_COLS; ++i) {
        all_pressed_keys[i] = Key();
    }

}

void Ze::Board::reset_pressed_keys() {
    for (uint8_t i = 0; i < MAX_NUM_KEYS; ++i) {
        curr_pressed_keys[i] = Key();
        just_released_keys[i] = Key();
    }

    for (uint8_t i = 0; i < NUM_ROWS * NUM_COLS; ++i) {
        if (all_pressed_keys[i].is_dummy()) break;
        all_pressed_keys[i] = Key();
    }
}

Ze::Key* Ze::Board::get_curr_pressed_keys() {
    return this->all_pressed_keys;
}

Ze::Key* Ze::Board::get_just_released_keys() {
    return this->just_released_keys;
}

uint8_t Ze::Board::get_num_keys_pressed() {
    return this->tot_num_keys_pressed;
}

uint8_t Ze::Board::get_num_released_keys() {
    return this->num_keys_released;
}

void Ze::Board::update() {
    reset_pressed_keys();
    num_keys_pressed = 0;
    num_keys_released = 0;
    tot_num_keys_pressed = 0;
    current_modifier = 0;
    pressed_media = Key();
    fn_pressed = false;
    fn2_pressed = false;

    scan_keys();

    update_keys_to_send();
    send_keys();

}

void Ze::Board::scan_keys() {
    for (uint8_t row = 0; row < NUM_ROWS; ++row) {

        // set this row to low
        digitalWrite(ROW_PINS[row], LOW);
        delayMicroseconds(READ_DELAY);

        for (uint8_t col = 0; col < NUM_COLS; ++col) {
            if (!KEYS[row][col].is_dummy()) {

                if (digitalRead(COL_PINS[col]) == LOW) {
                    // non dummy key is pressed

                  Key pressed = KEYS[row][col];
                    all_pressed_keys[tot_num_keys_pressed] = pressed;
                    tot_num_keys_pressed++;

                    if (pressed.is_fn()) {

                        this->fn_pressed = true;

                    } else if(pressed.is_fn2()) {

                        this->fn2_pressed = true;
                    
                    } else if (pressed.is_modifier()) {

                        current_modifier |= translate_modifier(pressed.code);

                    } else if (pressed.is_media()) {

                        pressed_media = pressed;

                    } else {

                        // if there is room for keys to send
                        if (num_keys_pressed < MAX_NUM_KEYS) {
                            curr_pressed_keys[num_keys_pressed] = pressed;
                        }
                        num_keys_pressed++;
                    }
                }
            }
        }
        digitalWrite(ROW_PINS[row], HIGH);
    }
}

void Ze::Board::remove_released_keys() {
    for (uint8_t i = 0; i < MAX_NUM_KEYS; ++i) {
        Key k = keys_to_send[i];

        if (!k.is_dummy()) {

            bool found = false;
            for (uint8_t j = 0; j < MAX_NUM_KEYS; ++j) {
                if (k == curr_pressed_keys[j]) {
                    found = true;
                    break;
                }
            }

            // if the key was not found in the
            // curr_pressed_keys array, it has
            // been released. Remove it from keys
            // to send and add it to just released keys.
            if (!found) {
                just_released_keys[num_keys_released] = keys_to_send[i];
                keys_to_send[i] = Key();
                codes_to_send[i] = KEY_DUMMY;
                num_keys_released++;
            }
        }
    }
}

void Ze::Board::update_keys_to_send() {
    remove_released_keys();
    for (uint8_t i = 0; i < MAX_NUM_KEYS; ++i) {
        Key k = curr_pressed_keys[i];
        if (!k.is_dummy()) {
            try_place_key(k);
        }
    }

    if (!pressed_media.is_dummy()) {
        if (pressed_media.has_second() && fn_pressed) {
            current_media = pressed_media.second;
        } else {
            current_media = pressed_media.code;
        }
    } else {
        current_media = 0;
    }
}

bool Ze::Board::try_place_key(Key& k) {
    uint8_t lowest_free_index = 0;
    bool found_lowest_index = false;
    for (uint8_t i = 0; i < MAX_NUM_KEYS; ++i) {

        // if this slot is empty and we haven't already found
        // a free slot
        if (keys_to_send[i].is_dummy() && !found_lowest_index) {
            // save the slot
            lowest_free_index = i;
            found_lowest_index = true;
        } else if (keys_to_send[i] == k) {
            // key is already sent, don't do anything
            return false;
        }
    }

    // the array was full
    if (!found_lowest_index) return false;

    uint8_t code;

    if (k.has_second() && fn_pressed) {
        code = k.second;
    } else if (k.has_third() && fn2_pressed) {
        code = k.third;
    } else {
        code = k.code;
    }

    keys_to_send[lowest_free_index] = k;

    codes_to_send[lowest_free_index] = code;

    return true;

}

void Ze::Board::send_keys() {

    Keyboard.set_key1(codes_to_send[0]);
    Keyboard.set_key2(codes_to_send[1]);
    Keyboard.set_key3(codes_to_send[2]);
    Keyboard.set_key4(codes_to_send[3]);
    Keyboard.set_key5(codes_to_send[4]);
    Keyboard.set_key6(codes_to_send[5]);

    Keyboard.set_modifier(current_modifier);

    Keyboard.set_media(current_media);

    Keyboard.send_now();
}
